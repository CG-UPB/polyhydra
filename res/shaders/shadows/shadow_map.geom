#version 400 core

#define MAX_CASCADES 8

layout (triangles, invocations = MAX_CASCADES) in;
layout (triangle_strip, max_vertices = 8) out;

flat in int v_Visible[3];

flat out int v_visible;

uniform mat4 u_light_space_matrices[16];

void vertex(vec4 pos, int visible)
{
    gl_Position = u_light_space_matrices[gl_InvocationID] * pos;
    gl_Layer = gl_InvocationID;
    v_visible = visible;
    EmitVertex();
}

void main()
{
    vertex(gl_in[0].gl_Position, v_Visible[0]);
    vertex(gl_in[1].gl_Position, v_Visible[1]);
    vertex(gl_in[2].gl_Position, v_Visible[2]);
    EndPrimitive();
}