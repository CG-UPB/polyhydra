#version 330 core

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Center;
layout (location = 3) in float a_peelDepth;
layout (location = 4) in float a_isBoundary;

out vec3 v_Pos;
out vec3 v_Normal;
flat out int v_Visible;

uniform mat4 u_Transform;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform vec3 u_lightPos;
uniform vec3 u_camPos;
uniform vec3 u_lightColor;
uniform vec3 u_objectColor;
uniform float u_cell_size;

uniform int u_peelDepth;
uniform int u_sliceCoord;
uniform float u_sliceMin;


void main()
{
    v_Visible = 1;
    float slicePos = (u_sliceCoord == 0) ? a_Center.x : (u_sliceCoord == 1) ? a_Center.y : (u_sliceCoord == 2) ? a_Center.z : 0.0;
    if (a_peelDepth < u_peelDepth || slicePos < u_sliceMin)
    {
        v_Visible = 0;
    }

//    if (u_cell_size == 1.0 && u_peelDepth == 0 && a_isBoundary == 0.0)
//    {
//        v_Visible = 0;
//    }

    vec3 pos = a_Center + (a_Pos - a_Center) * u_cell_size;
    v_Pos = vec3(u_Transform * vec4(pos, 1.0));
    v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
}