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
    FragColor = vec4(normalize(v_Normal), 1.0);

}