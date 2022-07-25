#version 330 core

const int MAX_CASCADE_LEVEL = 8;

in vec3 v_pos;
in vec3 v_normal;
in vec4 v_pos_ls[MAX_CASCADE_LEVEL];
in float v_clipspace_z;


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

uniform bool u_visible;


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

    float closest_depth = texture(u_shadow_texture[0], proj_coords.xy).r;
    float current_depth = proj_coords.z;

    for(int i = 0; i < 4; i++)
    {
        if(texture(u_shadow_texture[0], proj_coords.xy + poisson_disk[i] / 1000.0).r < current_depth - bias)
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

void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(u_viewport_width, u_viewport_height);
    if(!u_visible)
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

    // Phong Shading
    vec3 used_color = vec3(0.7, 0.7, 0.7);

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

    FragColor = vec4(result, 1.0);
}