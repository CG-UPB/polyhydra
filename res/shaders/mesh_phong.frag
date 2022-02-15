#version 330 core

out vec4 FragColor;

in vec3 v_pos;
in vec3 v_normal;
in vec4 v_color;
flat in int v_visible;

uniform vec3 u_lightPos;
uniform vec3 u_camPos;
uniform vec3 u_lightColor;
uniform vec3 u_objectColor;

void main()
{

    if (v_visible == 0)
    {
        discard;
    }
    //ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * u_lightColor;

    // Phong Shading

    vec3 used_color = mix(u_objectColor, vec3(v_color.x,v_color.y,v_color.z), v_color.w);

    //diffuse
    vec3 n = normalize(v_normal);
    vec3 l = normalize(u_lightPos - v_pos);
    float diff = max(0.0, dot(l, n));
    vec3 diffuse = diff * u_lightColor;

    //specular
    float specularStrength = 0.5;
    vec3 v = normalize(u_camPos - v_pos);
    vec3 r = reflect(-l, n);
    float spec = pow(max(0.0, dot(v, r)), 8);
    vec3 specular = specularStrength * spec * u_lightColor;

    vec3 result = (ambient + diffuse + specular) * used_color;

    FragColor = vec4(result, 1.0);
}