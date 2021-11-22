#version 330 core

in vec3 v_pos;
in vec3 v_normal;

uniform vec3 u_light_pos;
uniform vec3 u_cam_pos;
uniform vec3 u_light_color;
uniform vec4 u_base_color;
uniform bool u_phong;

out vec4 fragment_color;

void main()
{
    if (u_phong)
    {
        //ambient
        float ambientStrength = 0.3;
        vec3 ambient = ambientStrength * u_light_color;

        //diffuse
        vec3 n = normalize(v_normal);
        vec3 l = normalize(u_light_pos - v_pos);
        float diff = max(0.0, dot(l, n));
        vec3 diffuse = diff * u_light_color;

        //specular
        float specularStrength = 0.5;
        vec3 v = normalize(u_cam_pos - v_pos);
        vec3 r = reflect(-l, n);
        float spec = pow(max(0.0, dot(v,r)), 8);
        vec3 specular = specularStrength * spec * u_light_color;

        vec3 result = (ambient + diffuse + specular) * u_base_color.xyz;
        fragment_color = vec4(result, u_base_color.w);
    }
    else
    {
        fragment_color = u_base_color;
    }
}