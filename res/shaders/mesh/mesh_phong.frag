#version 330 core

in vec3 v_pos;
in vec3 v_normal;
flat in int v_visible;

uniform vec3 u_lightPos;
uniform vec3 u_camPos;
uniform vec3 u_lightColor;
uniform vec3 u_objectColor;

uniform int u_viewport_width;
uniform int u_viewport_height;

uniform sampler2D u_depth_texture;
uniform sampler2D u_ssao_texture;

out vec4 FragColor;

void main()
{

    if (v_visible == 0)
    {
        discard;
    }

    vec2 uv = gl_FragCoord.xy / vec2(u_viewport_width, u_viewport_height);

    //ambient
    float ambientStrength = 1.0;
    float ao_factor = texture(u_ssao_texture, uv).r;
    vec3 ambient = ambientStrength * u_lightColor * ao_factor;

    // Phong Shading

    //diffuse
    float diffuseStrength = 1.0;
    vec3 n = -normalize(v_normal);
    //vec3 l = normalize(u_lightPos - v_pos);
    // constant light direction looks way better than a single point of light
    vec3 l = normalize(vec3(0.0, -1.0, -1.0));
    float diff = max(0.0, dot(l, n));
    vec3 diffuse = diffuseStrength * diff * u_lightColor;

    //specular
    float specularStrength = 0.2;
    vec3 v = normalize(u_camPos - v_pos);
    vec3 r = reflect(-l, n);
    float spec = pow(max(0.0, dot(v, r)), 8);
    vec3 specular = specularStrength * spec * u_lightColor;

    vec3 result = (ambient + diffuse + specular) * u_objectColor;

    FragColor = vec4(result, 1.0);
}