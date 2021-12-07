#version 330 core

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;

out vec3 v_Pos;
out vec3 v_Normal;

uniform mat4 u_Transform;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform vec3 u_lightPos;
uniform vec3 u_camPos;
uniform vec3 u_lightColor;
uniform vec3 u_objectColor;

void main()
{
    v_Pos = vec3(u_Transform * vec4(a_Pos,1.0));
    v_Normal = a_Normal;
    gl_Position = u_Projection * u_View * u_Transform * vec4(a_Pos, 1.0);
}