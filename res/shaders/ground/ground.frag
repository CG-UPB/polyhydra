#version 330 core

#include "pbr.glsl"

const int MAX_CASCADE_LEVEL = 8;

in vec3 v_pos;
in vec3 v_normal;
in vec2 v_uv;
in vec4 v_pos_ls[MAX_CASCADE_LEVEL];
in float v_clipspace_z;

uniform bool u_visible;
uniform bool u_solid;
uniform vec3 u_solid_color;
uniform bool u_grid;
uniform vec3 u_grid_color;
uniform float u_height;
uniform int u_tile_count;
uniform bool u_shadow_only;
uniform bool u_wireframe;
uniform bool u_vertices;

uniform vec3 u_light_pos;
uniform float u_light_size = 1.0;
uniform vec3 u_cam_pos;
uniform vec3 u_light_color;

uniform bool u_draw_shadows;
uniform bool u_draw_ao;

uniform float u_spec_strength;
uniform float u_ambient_strength;
uniform float u_diffuse_strength;
uniform float u_spec_exponent;

uniform float u_shadow_strength;
uniform float u_softness;

uniform int u_cascade_level;
uniform float u_cascade_ends[MAX_CASCADE_LEVEL];
uniform mat4 u_view;

uniform int u_viewport_width;
uniform int u_viewport_height;

uniform float u_near;
uniform float u_far;

uniform float u_bias_min;
uniform float u_bias_max;
uniform float u_bias_modifier;

uniform sampler2D u_depth_texture;
uniform sampler2D u_ssao_texture;
uniform sampler2D u_transparent_shadow_texture;
uniform sampler2D u_color_filter_texture;
uniform sampler2DArray u_shadow_texture;

out vec4 FragColor;

vec2 poisson_disk[16] = vec2[](
    vec2( -0.94201624, -0.39906216 ),
    vec2( 0.94558609, -0.76890725 ),
    vec2( -0.094184101, -0.92938870 ),
    vec2( 0.34495938, 0.29387760 ),
    vec2( -0.91588581, 0.45771432 ),
    vec2( -0.81544232, -0.87912464 ),
    vec2( -0.38277543, 0.27676845 ),
    vec2( 0.97484398, 0.75648379 ),
    vec2( 0.44323325, -0.97511554 ),
    vec2( 0.53742981, -0.47373420 ),
    vec2( -0.26496911, -0.41893023 ),
    vec2( 0.79197514, 0.19090188 ),
    vec2( -0.24188840, 0.99706507 ),
    vec2( -0.81409955, 0.91437590 ),
    vec2( 0.19984126, 0.78641367 ),
    vec2( 0.14383161, -0.14100790 )
);


const vec2 Poisson25[25] = vec2[](
    vec2(-0.978698, -0.0884121),
    vec2(-0.841121, 0.521165),
    vec2(-0.71746, -0.50322),
    vec2(-0.702933, 0.903134),
    vec2(-0.663198, 0.15482),
    vec2(-0.495102, -0.232887),
    vec2(-0.364238, -0.961791),
    vec2(-0.345866, -0.564379),
    vec2(-0.325663, 0.64037),
    vec2(-0.182714, 0.321329),
    vec2(-0.142613, -0.0227363),
    vec2(-0.0564287, -0.36729),
    vec2(-0.0185858, 0.918882),
    vec2(0.0381787, -0.728996),
    vec2(0.16599, 0.093112),
    vec2(0.253639, 0.719535),
    vec2(0.369549, -0.655019),
    vec2(0.423627, 0.429975),
    vec2(0.530747, -0.364971),
    vec2(0.566027, -0.940489),
    vec2(0.639332, 0.0284127),
    vec2(0.652089, 0.669668),
    vec2(0.773797, 0.345012),
    vec2(0.968871, 0.840449),
    vec2(0.991882, -0.657338));

float frag_distance_to_screenspace_line(vec2 frag_pos, vec2 line_start, vec2 line_dir)
{
    vec2 af = frag_pos - line_start;
    return sqrt(dot(af, af) - dot(line_dir, af));
}

float random(vec3 seed, int i){
    vec4 seed4 = vec4(seed,i);
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}

float get_blocker_distance(vec3 shadow_coords, float bias, float light_size, int cascade_idx)
{
    int blockers = 0;
    float avg_blocker_distance = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(u_shadow_texture, 0));

    float search_width = light_size * (shadow_coords.z - 0.1) / shadow_coords.z;


    if(search_width < 0)
    {
        return 0.0;
    }

    int range = int(search_width);
    int samples = 2;
    for(int x = -samples; x <= samples; ++x)
    {
        for (int y = -samples; y <= samples; y++)
        {
            vec2 shift = vec2(x * 2.0  * range / samples, y * 2.0 * range / samples);
            float z = texture(u_shadow_texture, vec3(shadow_coords.xy + (vec2(x, y) + shift) * texelSize, float(cascade_idx))).r;
            if(z < (shadow_coords.z - bias))
            {
                blockers++;
                avg_blocker_distance += z;
            }
        }
    }

    if(blockers > 0)
    {
        return avg_blocker_distance / blockers;
    }
    else
    {
        return -1.0;
    }

}

float percentage_closer_filtering(vec3 shadow_coords, float light_size, float radius, float bias, int cascade_idx)
{
    float sum = 0;
    int count = 0;

    vec2 texelSize = 1.0 / vec2(textureSize(u_shadow_texture, 0));
    int range = int(light_size * radius);
   // range = range > 40 ? 40 : range;
    range = range <  1 ?  2 : range;

    for(int x = - range; x <= range; ++x)
    {
        count++;
        for (int y = -range; y <= range; ++y)
        {
            //int index = int(25.0 * random(gl_FragCoord.xyy, x)) % 25;
            //float depth = texture(u_shadow_texture, vec3(shadow_coords.xy + u_softness * vec2(x , y) * Poisson25[index]* texelSize, float(cascade_idx))).r;
            float depth = texture(u_shadow_texture, vec3(shadow_coords.xy + u_softness  * vec2(x , y) * texelSize, float(cascade_idx))).r;

            sum += depth < shadow_coords.z ? 1.0 : 0.0;
        }
    }

    count = count > 0 ? count : 1;

    return sum / (count * count);
}

float pcss_shadow_calculation(vec4 pos_ls, float light_size, float bias, int cascade_idx)
{
    vec3 shadow_coords = pos_ls.rgb / pos_ls.w;
    shadow_coords = shadow_coords * 0.5 + 0.5;


    if(shadow_coords.z > 1.0)
    {
        return 0.0;
    }

    //Step 1: Blocker search
    float blocker_distance = get_blocker_distance(shadow_coords, bias, light_size, cascade_idx);
    if(blocker_distance == -1.0)
        return 0.0;

    //Step 2: Penumbra estimation
    float penumbra_width = light_size * ((shadow_coords.z - blocker_distance) / blocker_distance);
    if(penumbra_width == 0.0)
        return 1.0;

    //Step 3: Filtering
    float radius = 2.0 * penumbra_width;
   // radius = light_size;

    float start = cascade_idx == 0 ? u_near : u_cascade_ends[cascade_idx - 1];
    float end = u_cascade_ends[cascade_idx];

    int next_cascade_idx = min(u_cascade_level, cascade_idx + 1);
    vec3 next_shadow_coords = (v_pos_ls[next_cascade_idx].xyz / v_pos_ls[next_cascade_idx].w) * 0.5 + 0.5;

    float fade = clamp((1.0 - v_clipspace_z / u_cascade_ends[cascade_idx]) / 0.05, 0.0, 1.0);

    float shadow = percentage_closer_filtering(shadow_coords, light_size, radius, bias, cascade_idx);

    if(fade < 1.0)
    {
        float next_shadow = percentage_closer_filtering(next_shadow_coords, light_size, radius, bias, next_cascade_idx);
        shadow = mix(shadow, next_shadow, fade);
    }

    return shadow;
}

float shadow_calculation(vec4 pos_ls, float bias, int cascade_idx)
{
    float shadow = 0.0;

    // range [-1, 1]
    vec3 proj_coords = pos_ls.xyz / pos_ls.w;

    // range [0, 1]
    proj_coords = proj_coords * 0.5 + 0.5;

    float current_depth = proj_coords.z;
    float closest_depth = texture(u_shadow_texture, vec3(proj_coords.xy, float(cascade_idx))).r;

    if (current_depth > 1.0)
    {
        return 0.0;
    }

    for(int i = 0; i < 4; i++)
    {
        int index = int(16.0 * random(gl_FragCoord.xyy, i)) % 16;
        //shadow += 0.25 * (1.0 - texture(u_shadow_texture, vec4(proj_coords.xy + (poisson_disk[i] / 700.0), float(cascade_idx), current_depth ))).r;
    }
    return shadow;
}

float linearize_depth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * u_near * u_far) / (u_far + u_near - z * (u_far - u_near));
}

void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(u_viewport_width, u_viewport_height);
    if(!u_visible || !(u_solid || u_grid))
    {
        discard;
    }

    vec3 light_color = u_light_color;
    vec3 n = normalize(v_normal);
    vec3 l = normalize(u_light_pos);

    float shadow = 0.0;
    bool no_shadows = u_wireframe || u_vertices;

    if (u_draw_shadows && !no_shadows)
    {
        // shadow calculation
        // calculate cascade level
        int cascade_idx = -1;
        int cascade_level = 1;
        if(u_cascade_level < MAX_CASCADE_LEVEL)
        {
            cascade_level = u_cascade_level;
        }
        else
        {
            cascade_level = MAX_CASCADE_LEVEL;
        }
        for(int i = 0; i < cascade_level ; ++i)
        {
            if(v_clipspace_z - u_near <= u_cascade_ends[i])
            {
                cascade_idx = i;
                //i = cascade_level;
                break;
            }
        }
        if(cascade_idx == -1)
        {
            cascade_idx = u_cascade_level;
        }

        // calculate bias (depending on cascade level)
        float bias = max(u_bias_max * (1.0f - max(0.0, dot(n, l))), u_bias_min);

        const float biasModifier = 0.5f;
        if (cascade_level == cascade_level)
        {
            bias *= 1 / (100.0 * biasModifier);
        }
        else
        {
            bias *= 1 / (u_cascade_ends[cascade_level] * biasModifier);
        }

        //bias *= 1.0 / (u_cascade_ends[cascade_level] * u_bias_modifier);

        //cascade_idx = 0;
        //shadow = u_shadow_strength * shadow_calculation(v_pos_ls[cascade_idx], bias, cascade_idx);
        shadow = u_shadow_strength * pcss_shadow_calculation(v_pos_ls[cascade_idx], u_light_size, bias, cascade_idx);
//
    }

    float ao_factor = 1.0;
    if(u_draw_ao && !no_shadows)
    {
        ao_factor = texture(u_ssao_texture, uv).r;
    }

    if (u_shadow_only)
    {
        if (shadow == 0.0 && ao_factor == 1.0)
        {
            discard;
        }
        vec3 out_color = vec3(0.25, 0.25, 0.25);
        if (shadow != 0.0)
        {
            FragColor = vec4(out_color * ao_factor, shadow * 0.25);
        }
        else
        {
            FragColor = vec4(out_color, 1.0 - ao_factor);
        }
        return;
    }

    float alpha = 1.0;
    vec3 color = vec3(0.0, 0.0, 0.0);

    vec2 coord = v_uv * float(u_tile_count);

    vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord) * 0.8;
    float line = min(grid.x, grid.y);

    float col = min(line, 1.0);
    col = pow(col, 1.0 / 1.2);

    if(dot((u_cam_pos - v_pos), v_normal) <= 0.0 )
    {
        shadow = 0.0;
        ao_factor = 1.0;
    }

    if(u_grid && u_solid)
    {
        color =  (1.0 - col) * u_grid_color + col * u_solid_color;
    }
    else if(u_grid && !u_solid)
    {
        color = u_grid_color;
        color = color - col * vec3(1.0 - color.x, 1.0 - color.y, 1.0 - color.z);
        alpha = 1.0 - col;
        if(col == 1.0)
        {
            discard;
        }
    }
    else if(!u_grid && u_solid)
    {
        color = u_solid_color;
    }

    vec3 result = color;
    if (u_use_pbr)
    {
        vec3 v = normalize(u_cam_pos - v_pos);
        vec3 light = normalize(u_light_pos);
        result = calculate_pbr_lighting(color, n, light, v, ao_factor, shadow, u_light_color);
    }
    else
    {
        result *= 1.0 - shadow * 0.5;
    }
    result = mix(result, u_light_color, linearize_depth(gl_FragCoord.z) / u_far - 0.5);
    result *= ao_factor;

    FragColor = vec4(result, alpha);
}
