#version 330 core

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

uniform bool u_draw_wireframe;
uniform bool u_draw_shadows;
uniform bool u_draw_ao;
uniform float u_wireframe_size;

uniform vec3 u_light_pos;
uniform vec3 u_cam_pos;
uniform vec3 u_light_color;
uniform vec4 u_object_color;

uniform int u_viewport_width;
uniform int u_viewport_height;

uniform float u_spec_strength;
uniform float u_ambient_strength;
uniform float u_diffuse_strength;
uniform float u_spec_exponent;

uniform float peel_depth;
uniform int u_cascade_level;
uniform float u_cascade_ends[MAX_CASCADE_LEVEL];
uniform mat4 u_view;

uniform float u_bias_min;
uniform float u_bias_max;
uniform float u_bias_modifier;


uniform sampler2D u_depth_texture;
uniform sampler2D u_ssao_texture;
uniform sampler2D u_transparent_shadow_texture;
uniform sampler2D u_color_filter_texture;
uniform sampler2D u_shadow_texture[MAX_CASCADE_LEVEL];

out vec4 FragColor;

vec2 poisson_disk[4] = vec2[](
    vec2(-0.94201624, -0.39906216),
    vec2(0.94558609, -0.76890725),
    vec2(-0.094184101, -0.92938870),
    vec2(0.34495938, 0.29387760)
);

float frag_distance_to_screenspace_line(vec2 frag_pos, vec2 line_start, vec2 line_dir)
{
    vec2 af = frag_pos - line_start;
    return sqrt(dot(af, af) - dot(line_dir, af));
}

float shadow_calculation(vec4 pos_ls, float bias, int cascade_idx)
{
    float shadow = 0.0;

    // range [-1, 1]
    vec3 proj_coords = pos_ls.xyz / pos_ls.w;

    // range [0, 1]
    proj_coords = proj_coords * 0.5 + 0.5;

    float closest_depth = texture(u_shadow_texture[cascade_idx], proj_coords.xy).r;
    float current_depth = proj_coords.z;
    //shadow = current_depth - bias > closest_depth ? 1.0 : 0.0;

    // sample surrounding values and use average value for smoother shadows
//    vec2 texel_size = 1.0 / textureSize(u_shadow_texture, 0);
//    for(int x = -1; x <= 1; ++x)
//    {
//        for(int y = -1; y <= 1; ++y)
//        {
//            float pcf_depth = texture(u_shadow_texture, proj_coords.xy + vec2(x, y) * texel_size).r;
//            shadow += current_depth - bias > pcf_depth ? 1.0 : 0.0;
//        }
//    }
//    shadow /= 9.0;


    for(int i = 0; i < 4; i++)
    {
        if(texture(u_shadow_texture[cascade_idx], proj_coords.xy + poisson_disk[i] / 1000.0).r < current_depth - bias)
        {
            shadow += 0.25;
        }
    }


    if(proj_coords.z > 1.0)
    {
        shadow = 0.0;
    }

    return shadow;
}

float transparent_shadow_calculation(vec4 pos_ls, float bias)
{
    // range [-1, 1]
    vec3 proj_coords = pos_ls.xyz / pos_ls.w;

    // range [0, 1]
    proj_coords = proj_coords * 0.5 + 0.5;

    float closest_depth = texture(u_transparent_shadow_texture, proj_coords.xy).r;
    float current_depth = proj_coords.z;

    float shadow = current_depth - bias > closest_depth ? 1.0 : 0.0;

    return shadow;
}

vec3 color_filter(vec4 pos_ls)
{
    // range [-1, 1]
    vec3 proj_coords = pos_ls.xyz / pos_ls.w;

    // range [0, 1]
    proj_coords = proj_coords * 0.5 + 0.5;

    vec4 color = texture(u_color_filter_texture, proj_coords.xy);

    return  (1 - color.a) *  color.rgb;
}

void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(u_viewport_width, u_viewport_height);
    if (u_draw_wireframe)
    {
        if (v_visible == 0)
        {
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
            float min_dist_to_edge = min(min(v_tri_dist.x, v_tri_dist.y), v_tri_dist.z);
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
        return;
    }

    // if face is not visible or transparent: Discard fragment
    // Transparency gets handled in another pass
    if (v_visible == 0 || v_color.a < 1.0 - 0.01)
    {
        discard;
    }

#if 0 // show wireframe on top of the mesh, comment out the other wireframe stuff on top or this won't work
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
        return;
    }
#endif

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

//        float transparent_shadow = transparent_shadow_calculation(v_pos_ls, bias);
//        transparent_shadow = 0.0;
//        if (transparent_shadow != 0.0)
//        {
//            if (shadow == 0.0)
//            {
//                // if pixel only lays in transparent shadow, then apply color filter
//                light_color = light_color * color_filter(v_pos_ls);
//                shadow = 1.0;
//            }
//        }
    }

    // Phong Shading
    vec3 used_color = mix(u_object_color.rgb, v_color.rgb, v_color.a);

    //ambient
    float ao_factor = 1.0;
    if(u_draw_ao)
    {
        ao_factor = texture(u_ssao_texture, uv).r;
    }
    vec3 ambient = u_ambient_strength * light_color * ao_factor;

    //diffuse
    float diff = max(0.0, dot(l, n));
    vec3 diffuse = u_diffuse_strength * diff * light_color;

    //specular
    vec3 v = normalize(u_cam_pos - v_pos);
    vec3 r = reflect(-l, n);
    float spec = pow(max(0.0, dot(v, r)), u_spec_exponent);
    vec3 specular = u_spec_strength * spec * light_color;

    float norm = u_ambient_strength + u_diffuse_strength + u_spec_strength;
    vec3 result = (ambient + (1.0 - shadow + 0.2) * (diffuse + specular)) / norm * used_color;

    FragColor = vec4(result, v_color.a);
}