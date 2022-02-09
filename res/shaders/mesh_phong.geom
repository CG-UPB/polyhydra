#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 8) out;

in vec3 v_Pos[3];
in vec3 v_Normal[3];
in vec3 v_Color[3];
flat in int v_Visible[3];

uniform mat4 u_Transform;
uniform mat4 u_Projection;
uniform mat4 u_View;

out vec3 v_pos;
out vec3 v_normal;
out vec3 v_color;
flat out int v_visible;

void vertex(vec3 pos, vec3 normal, vec3 color, int visible)
{
    gl_Position = u_Projection * u_View * vec4(pos, 1.0);
    v_pos = pos;
    v_normal = normal;
    v_color = color;
    v_visible = visible;
    EmitVertex();
}

void main()
{
    vec3 pos0 = v_Pos[0];
    vec3 pos1 = v_Pos[1];
    vec3 pos2 = v_Pos[2];
    vertex(pos0, v_Normal[0], v_Color[0], v_Visible[0]);
    vertex(pos1, v_Normal[1], v_Color[1], v_Visible[1]);
    vertex(pos2, v_Normal[2], v_Color[2], v_Visible[2]);
    EndPrimitive();
}