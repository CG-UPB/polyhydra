#version 330 core

#include "pbr.glsl"
#include "phong.glsl"

const int MAX_CASCADE_LEVEL = 8;

in vec3 v_pos;
in vec3 v_normal;
in vec4 v_color;
in vec4 v_pos_ls[MAX_CASCADE_LEVEL];
in float v_clipspace_z;
flat in int v_visible;
in vec3 v_tri_dist;
flat in int v_is_triangle;
flat in vec4 v_a_adir;
flat in vec4 v_b_bdir;
flat in int v_use_lookup_path;
flat in int v_tes_inner_tri;

uniform bool u_draw_wireframe;
uniform bool u_draw_shadows;
uniform bool u_draw_ao;
uniform float u_wireframe_size;

uniform vec3 u_light_pos;
uniform float u_light_size = 1.0;
uniform vec3 u_cam_pos;
uniform vec3 u_view_dir;
uniform vec3 u_light_color;
uniform vec4 u_object_color;

uniform int u_viewport_width;
uniform int u_viewport_height;
uniform float u_near;
uniform float u_far;

uniform bool  u_use_base_color;
uniform bool  u_two_sided_lighting;

uniform float u_shadow_strength;
uniform float u_softness;

uniform float peel_depth;
uniform int u_cascade_level;
uniform float u_cascade_ends[MAX_CASCADE_LEVEL];
uniform mat4 u_view;

uniform float u_bias_min;
uniform float u_bias_max;
uniform float u_bias_modifier;

// uniforms for bezier meshes
uniform bool u_is_bezier_mesh;

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

float frag_distance_to_screenspace_line(vec2 frag_pos, vec2 line_start, vec2 line_dir)
{
    vec2 af = frag_pos - line_start;
    return sqrt(dot(af, af) - dot(line_dir, af));
}

void draw_wireframe(vec2 uv)
{
    if (v_visible == 0)
    {
        discard;
    }
    if(u_is_bezier_mesh && v_tes_inner_tri == 1)
    {
        // Discard all inner triangles of a tessellated triangle.
        discard;
    }
    float size_factor = 0.0015 * u_wireframe_size;
    if (v_use_lookup_path == 1)
    {
        // these triangles are very likely not visible, since we don't draw 2/3rds of those anyway
        if (v_is_triangle == 0)
        {
            discard;
        }
        else
        {
            float dist0 = frag_distance_to_screenspace_line(uv, v_a_adir.xy, v_a_adir.zw);
            float dist1 = frag_distance_to_screenspace_line(uv, v_b_bdir.xy, v_b_bdir.zw);
            if (v_a_adir.xy != v_b_bdir.xy)
            {
                float dist2 = frag_distance_to_screenspace_line(uv, v_a_adir.xy, normalize(v_b_bdir.xy - v_a_adir.xy));
                float min_dist_to_edge = min(min(dist0, dist1), dist2);
                if (min_dist_to_edge > size_factor)
                {
                    discard;
                }
            }
            else
            {
                float min_dist_to_edge = min(dist0, dist1);
                if (min_dist_to_edge > size_factor)
                {
                    discard;
                }
            }
        }
    }
    else
    {
        float min_dist_to_edge;
        if(!u_is_bezier_mesh)
        {
            min_dist_to_edge = min(min(v_tri_dist.x, v_tri_dist.y), v_tri_dist.z);
        }
        else
        {
            // For Bézier meshes, draw an outline only for outer edges.
            min_dist_to_edge = v_tri_dist.y;
        }

        if (min_dist_to_edge > size_factor)
        {
            discard;
        }
        // here, we discard 2 of our 3 edges that we added in our triangulation, since only want to draw the original edges
        if (v_is_triangle == 0 && (min_dist_to_edge == v_tri_dist.x || min_dist_to_edge == v_tri_dist.z) && v_tri_dist.y > size_factor)
        {
            discard;
        }
    }
    FragColor = vec4(u_object_color.rgb, 1.0);
}

void draw_wireframe_ontop(vec2 uv)
{
    bool fragment_in_wireframe = true;
    float size_factor = 0.0015 * u_wireframe_size;
    if (v_use_lookup_path == 1)
    {
        // these triangles are very likely not visible, since we don't draw 2/3rds of those anyway
        if (v_is_triangle == 0)
        {
            fragment_in_wireframe = false;
        }
        else
        {
            float dist0 = frag_distance_to_screenspace_line(uv, v_a_adir.xy, v_a_adir.zw);
            float dist1 = frag_distance_to_screenspace_line(uv, v_b_bdir.xy, v_b_bdir.zw);
            if (v_a_adir.xy != v_b_bdir.xy)
            {
                float dist2 = frag_distance_to_screenspace_line(uv, v_a_adir.xy, normalize(v_b_bdir.xy - v_a_adir.xy));
                float min_dist_to_edge = min(min(dist0, dist1), dist2);
                if (min_dist_to_edge > size_factor)
                {
                    fragment_in_wireframe = false;
                }
            }
            else
            {
                float min_dist_to_edge = min(dist0, dist1);
                if (min_dist_to_edge > size_factor)
                {
                    fragment_in_wireframe = false;
                }
            }
        }
    }
    else
    {
        float min_dist_to_edge = min(min(v_tri_dist.x, v_tri_dist.y), v_tri_dist.z);
        if (min_dist_to_edge > size_factor)
        {
            fragment_in_wireframe = false;
        }
        // here, we discard 2 of our 3 edges that we added in our triangulation, since only want to draw the original edges
        if (v_is_triangle == 0 && (min_dist_to_edge == v_tri_dist.x || min_dist_to_edge == v_tri_dist.z) && v_tri_dist.y > size_factor)
        {
            fragment_in_wireframe = false;
        }
    }
    if (fragment_in_wireframe)
    {
        FragColor = vec4(u_object_color.rgb * 0.5, 1.0);
    }
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
    int samples = 4;
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

    range = range > 10 ? 10 : range;
    range = range <  1 ?  2 : range;

    for(int x = - range; x <= range; ++x)
    {
        count++;
        for (int y = -range; y <= range; ++y)
        {
            //int index = int(25.0 * random(gl_FragCoord.xyy, x)) % 25;
            //float depth = texture(u_shadow_texture, vec3(shadow_coords.xy + u_softness * vec2(x , y) * Poisson25[index]* texelSize, float(cascade_idx))).r;
            float depth = texture(u_shadow_texture, vec3(shadow_coords.xy + vec2(x , y) * texelSize, float(cascade_idx))).r;

            sum += depth < shadow_coords.z - bias? 1.0 : 0.0;
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

//    //Step 1: Blocker search
//    float blocker_distance = get_blocker_distance(shadow_coords, bias, light_size, cascade_idx);
//    if(blocker_distance == -1.0)
//        return 0.0;
//
//    //Step 2: Penumbra estimation
//    float penumbra_width = light_size * ((shadow_coords.z - blocker_distance) / blocker_distance);
//    if(penumbra_width == 0.0)
//        return 1.0;

    //Step 3: Filtering
    float radius = 1.0;
    // radius = light_size;

    float shadow = percentage_closer_filtering(shadow_coords, light_size, radius, bias, cascade_idx);

    return shadow;
}

float shadow_calculation(vec4 pos_ls, float bias, int cascade_idx)
{
    float shadow = 0.0;

    vec3 n = normalize(v_normal);
    // range [-1, 1]
    vec3 proj_coords = pos_ls.xyz / pos_ls.w;

    // range [0, 1]
    proj_coords = proj_coords * 0.5 + 0.5;

    float current_depth = proj_coords.z - bias;
    float closest_depth = texture(u_shadow_texture, vec3(proj_coords.xy, float(cascade_idx))).r;

    if (current_depth > 1.0)
    {
        return 0.0;
    }

    for(int i = 0; i < 4; i++)
    {
        int index = int(16.0 * random(gl_FragCoord.xyy, i)) % 16;
        float depth = texture(u_shadow_texture, vec3(proj_coords.xy + (poisson_disk[i] / 1000.0) * 0.4, float(cascade_idx))).r;
        if(depth < current_depth)
        {
            shadow += 0.25;
        }

    }
    return shadow;
}

float get_shadow(vec3 normal, vec3 light_dir)
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
    float light_angle = clamp(dot(normal, light_dir), 0.0, 1.0);
    //shadow = max(light_angle, 0.0);
    float bias = max(u_bias_max * (1.0f - max(0.0, dot(normal, light_dir))), u_bias_min);

    //const float biasModifier = 0.5f;
    //bias *= 1 / (u_cascade_ends[cascade_idx] * biasModifier);


    //return shadow_calculation(v_pos_ls[cascade_idx], bias, cascade_idx);
    return pcss_shadow_calculation(v_pos_ls[cascade_idx], u_light_size, bias, cascade_idx);
}

void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(u_viewport_width, u_viewport_height);
//    if (u_draw_wireframe)
//    {
//        draw_wireframe(uv);
//        return;
//    }

    // if face is not visible or transparent: Discard fragment
    // Transparency gets handled in another pass
    float alpha = v_color.a;
    if (v_visible == 0 || alpha < 1.0 - 0.00001)
    {
        discard;
    }

#if 0 // show wireframe on top of the mesh, comment out the other wireframe stuff on top or this won't work
    //draw_wireframe_ontop(uv);
    //return;
#endif

    vec3 n = normalize(v_normal);
    vec3 l = normalize(u_light_pos);
    vec3 v = normalize(u_cam_pos - v_pos);
    if(u_two_sided_lighting && dot(n, l) < 0 )
    {
        n = -n;
    }

    if(u_is_bezier_mesh  && dot(n, v) < 0 )
    {
        n = -n;
    }


    float shadow = 0.0;
//    if (u_draw_shadows)
//    {
//        shadow = get_shadow(n, l);
//        shadow = shadow * u_shadow_strength;
//    }

    // Phong Shading
    vec4 used_color = vec4(1.0f);
    used_color = u_use_base_color ? u_object_color : v_color;

    //ambient
    float ao_factor = 1.0;
    if (u_draw_ao)
    {
        ao_factor = texture(u_ssao_texture, uv).r;
    }

    vec3 result;
    if (u_use_pbr)
    {
        result = calculate_pbr_lighting(used_color.rgb, n, l, v, ao_factor, shadow, u_light_color);
    }
    else
    {
        result = calculate_phong_lighting(used_color.rgb, n, l, v, ao_factor, shadow, u_light_color);
    }
    FragColor = vec4(result, used_color.a);
}
