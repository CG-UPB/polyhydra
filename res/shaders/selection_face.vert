#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_center;
layout (location = 3) in float a_peelDepth;

uniform mat4 u_mesh_transform;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform float u_cell_size;

uniform int u_peelDepth;
uniform int u_sliceCoord;
uniform float u_sliceMin;

flat out int v_visible;

void main()
{
    v_visible = 1;
    float slicePos = (u_sliceCoord == 0) ? a_center.x : (u_sliceCoord == 1) ? a_center.y : (u_sliceCoord == 2) ? a_center.z : 0.0;
    if (a_peelDepth < u_peelDepth || slicePos < u_sliceMin)
    {
        v_visible = 0;
    }

    vec3 pos = a_center + (a_pos - a_center) * u_cell_size;
    gl_Position = u_projection * u_view * u_mesh_transform * vec4(pos, 1.0);
}