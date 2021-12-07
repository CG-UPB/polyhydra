#version 330 core

out vec4 FragColor;

in vec3 v_Pos;
in vec3 v_Normal;

uniform vec3 u_lightPos;
uniform vec3 u_camPos;
uniform vec3 u_lightColor;
uniform vec3 u_objectColor;

void main()
{
    //ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * u_lightColor;

        // Phong Shading

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
    
}