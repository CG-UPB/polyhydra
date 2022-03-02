#version 400 core

layout (location = 0) out vec4 FragColor;

in vec3 v_pos;
in vec3 v_normal;
in vec4 v_color;
flat in int v_visible;

uniform vec3 u_lightPos;
uniform vec3 u_camPos;
uniform vec3 u_lightColor;
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

uniform sampler2D last_depth_texture;
uniform sampler2D u_ssao_texture;

void main()
{
    vec4 color = u_object_color;
    float frag_depth = gl_FragCoord.z;

    float last_depth = texelFetch(last_depth_texture, ivec2(gl_FragCoord.xy), 0).r;

    if(frag_depth >= last_depth || color.a >= 1.0 || v_visible == 0)
    {
        discard;
    }

    vec2 uv = gl_FragCoord.xy / vec2(u_viewport_width, u_viewport_height);

    //ambient
    float ambientStrength = 1.0;
    float ao_factor = texture(u_ssao_texture, uv).r;
    vec3 ambient = ambientStrength * u_lightColor;

    // Phong Shading

    vec3 used_color = mix(u_object_color.rgb, vec3(v_color.x,v_color.y,v_color.z), v_color.w);

    //diffuse
    float diffuseStrength = 1.0;
    vec3 n = -normalize(v_normal);

    //vec3 l = normalize(u_lightPos - v_pos);
    // constant light direction looks way better than a single point of light
    vec3 l = normalize(vec3(0.0, -1.0, -1.0));
    float diff = max(0.0, dot(l, n));
    vec3 diffuse = diffuseStrength * diff * u_lightColor;
//
//    if(dot(n,l) < 0)
//    {
//        n = -n;
//    }

    //specular
    float specularStrength = 0.2;
    vec3 v = normalize(u_camPos - v_pos);
    vec3 r = reflect(-l, n);
    float spec = pow(max(0.0, dot(v, r)), 8);
    vec3 specular = specularStrength * spec * u_lightColor;

    float norm = ambientStrength + diffuseStrength + specularStrength;
    color.rgb = (ambient + diffuse + specular) / norm * used_color;

    FragColor = color;
    //FragColor = vec4(vec3(last_depth), 1.0);

}