#version 330 core

layout (location = 0) in vec3 a_Pos;
//layout (location = 1) in vec3 a_Normal
//[x,y,z, xn, yn, zn,

uniform mat4 u_Transform;
uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
    vec4 position = vec4(a_Pos.x, a_Pos.y, a_Pos.z, 1.0);
    gl_Position = u_Projection * u_View * u_Transform * position;
}