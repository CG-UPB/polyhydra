#version 400 core

layout (location = 0) out vec4 FragColor;

in vec3 v_pos;
in vec3 v_normal;
in vec4 v_color;
flat in int v_visible;

uniform vec3 u_light_pos;
uniform vec3 u_cam_pos;
uniform vec3 u_light_color;
uniform vec4 u_object_color;
uniform int u_viewport_width;
uniform int u_viewport_height;

uniform float u_alpha_pow;
uniform float u_pow;
uniform float u_range;
uniform float u_depth_range;
uniform float u_ordering_strenth;
uniform float u_t_min;
uniform float u_t_max;

uniform float u_spec_strength;
uniform float u_ambient_strength;
uniform float u_diffuse_strength;
uniform float u_spec_exponent;

uniform sampler2D last_depth_texture;

void main()
{
    vec4 color = u_object_color;
    float frag_depth = gl_FragCoord.z;

    float last_depth = texelFetch(last_depth_texture, ivec2(gl_FragCoord.xy), 0).r;

    if(frag_depth >= last_depth || color.a >= 1.0 || v_visible == 0)
    {
        discard;
    }
    vec3 light_color = u_light_color;
    vec3 n = normalize(v_normal);
    vec3 l = normalize(u_light_pos - v_pos);

    vec2 uv = gl_FragCoord.xy / vec2(u_viewport_width, u_viewport_height);

    // Phong Shading
    vec3 used_color = mix(u_object_color.rgb, v_color.rgb, v_color.a);

    //ambient
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
    color.rgb = (ambient + (diffuse + specular)) / norm * used_color;

    FragColor = color;

}