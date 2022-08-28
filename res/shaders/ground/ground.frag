#version 330 core

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

uniform vec3 u_light_pos;
uniform vec3 u_cam_pos;
uniform vec3 u_light_color;

uniform bool u_draw_shadows;
uniform bool u_draw_ao;

uniform float u_spec_strength;
uniform float u_ambient_strength;
uniform float u_diffuse_strength;
uniform float u_spec_exponent;

uniform int u_cascade_level;
uniform float u_cascade_ends[MAX_CASCADE_LEVEL];
uniform mat4 u_view;

uniform int u_viewport_width;
uniform int u_viewport_height;

uniform float u_bias_min;
uniform float u_bias_max;
uniform float u_bias_modifier;

uniform sampler2D u_depth_texture;
uniform sampler2D u_ssao_texture;
uniform sampler2D u_transparent_shadow_texture;
uniform sampler2D u_color_filter_texture;
uniform sampler2DArrayShadow u_shadow_texture;

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

float shadow_calculation(vec4 pos_ls, float bias, int cascade_idx)
{
    float shadow = 0.0;

    // range [-1, 1]
    vec3 proj_coords = pos_ls.xyz / pos_ls.w;

    // range [0, 1]
    proj_coords = proj_coords * 0.5 + 0.5;

    float closest_depth = texture(u_shadow_texture, vec4(proj_coords.xy, float(cascade_idx), proj_coords.z));
    float current_depth = proj_coords.z;
    if (current_depth > 1.0)
    {
        return 0.0;
    }

    for(int i = 0; i < 4; i++)
    {
        int index = int(16.0 * random(gl_FragCoord.xyy, i)) % 16;
        shadow += 0.2 * (1.0 - texture(u_shadow_texture, vec4(proj_coords.xy + (poisson_disk[i] / 1000.0) * 0.4, float(cascade_idx), proj_coords.z)));
    }
    return shadow;
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
    if (u_draw_shadows)
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
            if(v_clipspace_z <= u_cascade_ends[i])
            {
                cascade_idx = i;
                i = cascade_level;
                break;
            }
        }
        if(cascade_idx == -1)
        {
            cascade_idx = u_cascade_level;
        }

        // calculate bias (depending on cascade level)
        float bias = max(u_bias_max * (1.0f - max(0.0, dot(n, l))), u_bias_min);

        //bias *= 1.0 / (u_cascade_ends[cascade_level] * u_bias_modifier);

        //cascade_idx = 0;
        shadow = shadow_calculation(v_pos_ls[cascade_idx], bias, cascade_idx);
    }

    float ao_factor = 1.0;
    if(u_draw_ao)
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
        color = (1.0 - shadow * 0.5) * u_solid_color;
        color =  (1.0 - col) * u_grid_color + col * color;
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
        vec3 ambient = u_ambient_strength * light_color;

        //diffuse
        float diff = max(0.0, dot(l, n));
        vec3 diffuse = u_diffuse_strength * diff * light_color;

        //specular
        vec3 v = normalize(u_cam_pos - v_pos);
        vec3 r = reflect(-l, n);
        float spec = pow(max(0.0, dot(v, r)), u_spec_exponent);
        vec3 specular = u_spec_strength * spec * light_color;

        float norm = u_ambient_strength + u_diffuse_strength + u_spec_strength;
        color = (1.0 - shadow) * color;
    }
    color *= ao_factor;
    FragColor = vec4(color, alpha);
}