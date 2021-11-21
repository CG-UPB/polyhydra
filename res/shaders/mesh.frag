#version 330 core

out vec4 FragColor;

in vec3 v_Pos;
in vec3 v_Normal;

uniform mat4 u_Transform;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform vec3 u_lightPos;
uniform vec3 u_camPos;
uniform vec3 u_lightColor;
uniform vec3 u_objectColor;
uniform bool u_phong;

void main()
{

    if(u_phong)
    {
        //ambient
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * u_lightColor;

        //diffuse
        vec3 n = normalize(v_Normal);
        vec3 l = normalize(u_lightPos - v_Pos);
        float diff = max(0.0, dot(l, n));
        vec3 diffuse = diff * u_lightColor;

        //specular
        float specularStrength = 0.5;
        vec3 v = normalize(u_camPos - v_Pos);
        vec3 r = reflect(-l, n);
        float spec = pow(max(0.0, dot(v,r)), 8);
        vec3 specular = specularStrength * spec * u_lightColor;

        vec3 result = (ambient + diffuse + specular) * u_objectColor;


        FragColor = vec4(result, 1.0);
    }else
    {
        FragColor = vec4(u_objectColor, 1.0);
    }

}