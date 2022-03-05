#version 330 core

in vec3 v_pos;
in vec3 v_normal;
in vec4 v_color;
in vec4 v_pos_ls;
flat in int v_visible;

in vec3 v_tri_dist;

uniform bool u_draw_wireframe;

uniform vec3 u_lightPos;
uniform vec3 u_camPos;
uniform vec3 u_lightColor;
uniform vec4 u_objectColor;

uniform int u_viewport_width;
uniform int u_viewport_height;

uniform float u_spec_strength;
uniform float u_ambient_strength;
uniform float u_diffuse_strength;
uniform float u_spec_exponent;

uniform sampler2D u_depth_texture;
uniform sampler2D u_ssao_texture;
uniform sampler2D u_shadow_texture;
uniform sampler2D u_transparent_shadow_texture;
uniform sampler2D u_color_filter_texture;

out vec4 FragColor;

float shadow_calculation(vec4 pos_ls, float bias)
{
    float shadow = 0.0;

    // range [-1, 1]
    vec3 proj_coords = pos_ls.xyz / pos_ls.w;

    // range [0, 1]
    proj_coords = proj_coords * 0.5 + 0.5;

    float closest_depth = texture(u_shadow_texture, proj_coords.xy).r;
    float current_depth = proj_coords.z;
//    float shadow = current_depth - bias > closest_depth ? 1.0 : 0.0;

    // sample surrounding values and use average value for smoother shadows
    vec2 texel_size = 1.0 / textureSize(u_shadow_texture, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcf_depth = texture(u_shadow_texture, proj_coords.xy + vec2(x, y) * texel_size).r;
            shadow += current_depth - bias > pcf_depth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

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

    if (u_draw_wireframe)
    {
        if (v_visible == 0)
        {
            discard;
        }
        float min_dist_to_edge = min(min(v_tri_dist.x, v_tri_dist.y), v_tri_dist.z);
        float max_dist_to_edge = max(max(v_tri_dist.x, v_tri_dist.y), v_tri_dist.z);
        if (min_dist_to_edge > 0.0015)
        {
            discard;
        }
        if (max_dist_to_edge > 2.0 && (min_dist_to_edge == v_tri_dist.x || min_dist_to_edge == v_tri_dist.z))
        {
            discard;
        }
        FragColor = vec4(u_objectColor.rgb, 1.0);
        return;
    }

    // if face is not visible or transparent: Discard fragment
    // Transparency gets handled in another pass
    if (v_visible == 0 || u_objectColor.a < 1.0 - 0.001)
    {
        discard;
    }
    vec3 light_color = u_lightColor;
    vec3 n = -normalize(v_normal);
    vec3 l = normalize(vec3(0.0, -1.0, -1.0));
    vec3 light_dir = normalize(u_lightPos - v_pos);
    float diff = max(0.0, dot(l, n));

    vec2 uv = gl_FragCoord.xy / vec2(u_viewport_width, u_viewport_height);

    // shadow calculation
    float bias = max(0.00005 * (1.0 - dot(n, light_dir)), 0.000005);
    float shadow = shadow_calculation(v_pos_ls, bias);
    float transparent_shadow = transparent_shadow_calculation(v_pos_ls, bias);
    transparent_shadow = 0.0;
    if(transparent_shadow != 0.0)
    {
        if(shadow == 0.0)
        {
            // if pixel only lays in transparent shadow, then apply color filter
            light_color = light_color * color_filter(v_pos_ls);
            shadow = 1.0;
        }
    }
    //shadow = 0.0;

    //ambient
    float ao_factor = texture(u_ssao_texture, uv).r;
    vec3 ambient = u_ambient_strength * light_color * ao_factor;

    // Phong Shading

    vec3 used_color = mix(u_objectColor.rgb, v_color.rgb, 0.0);

    //diffuse
    //vec3 l = normalize(u_lightPos - v_pos);
    // constant light direction looks way better than a single point of light
    vec3 diffuse = u_diffuse_strength * diff * light_color;

    //specular
    vec3 v = normalize(u_camPos - v_pos);
    vec3 r = reflect(-l, n);
    float spec = pow(max(0.0, dot(v, r)), u_spec_exponent);
    vec3 specular = u_spec_strength * spec * light_color;

    float norm = u_ambient_strength + u_diffuse_strength + u_spec_strength;
    vec3 result = (ambient + (1.0 - shadow + 0.2) * (diffuse + specular)) / norm * used_color;

    FragColor = vec4(result, 1.0);
}