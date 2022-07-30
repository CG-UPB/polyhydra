#version 330 core

in vec3 v_pos;
in vec3 v_normal;
in vec3 v_color;
flat in int v_visible;

uniform vec3 u_light_pos;
uniform vec3 u_cam_pos;
uniform vec3 u_light_color;

out vec4 fragment_color;

void main()
{
    if (v_visible == 0)
    {
        discard;
    }

    //ambient
    float ambientStrength = 0.8;
    vec3 ambient = ambientStrength * u_light_color;

    //diffuse
    float diffuseStrength = 1.0;
    vec3 n = normalize(v_normal);
    vec3 l = normalize(u_light_pos);
    float diff = max(0.0, dot(l, n));
    vec3 diffuse = diffuseStrength * diff * u_light_color;

    //specular
    float specularStrength = 0.1;
    vec3 v = normalize(u_cam_pos - v_pos);
    vec3 r = reflect(-l, n);
    float spec = pow(max(0.0, dot(v,r)), 8);
    vec3 specular = specularStrength * spec * u_light_color;

    vec3 result = (ambient + diffuse + specular) * v_color;
    fragment_color = vec4(result, 1.0);
}