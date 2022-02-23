#version 330 core

#define FLT_MAX 3.402823466e+38

layout (triangles) in;
layout (triangle_strip, max_vertices = 8) out;

in vec3 v_Pos[3];
in vec3 v_Normal[3];
in vec4 v_Color[3];
flat in int v_Visible[3];
flat in int v_isTriangle[3];

uniform mat4 u_Transform;
uniform mat4 u_Projection;
uniform mat4 u_View;

out vec3 v_pos;
out vec3 v_normal;
out vec4 v_color;
flat out int v_visible;

out vec3 v_tri_dist;

float dist_to_edge(vec3 e0, vec3 e1, vec3 p)
{
    return length(cross(p - e0, p - e1)) / length(e1 - e0);
}

void vertex(vec4 screen_pos, vec3 pos, vec3 normal, vec4 color, int visible)
{
    gl_Position = screen_pos;
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

    vec4 screen_pos0 = u_Projection * u_View * vec4(pos0, 1.0);
    vec4 screen_pos1 = u_Projection * u_View * vec4(pos1, 1.0);
    vec4 screen_pos2 = u_Projection * u_View * vec4(pos2, 1.0);

    vec3 view_pos0 = screen_pos0.xyz / screen_pos0.w;
    vec3 view_pos1 = screen_pos1.xyz / screen_pos1.w;
    vec3 view_pos2 = screen_pos2.xyz / screen_pos2.w;

    float dist1 = (v_isTriangle[1] == 0) ? FLT_MAX : dist_to_edge(view_pos0, view_pos2, view_pos1);
    float dist2 = (v_isTriangle[2] == 0) ? FLT_MAX : dist_to_edge(view_pos0, view_pos1, view_pos2);

    v_tri_dist = vec3(0.0, dist_to_edge(view_pos1, view_pos2, view_pos0), 0.0);
    vertex(screen_pos0, pos0, v_Normal[0], v_Color[0], v_Visible[0]);

    v_tri_dist = vec3(0.0, 0.0, dist1);
    vertex(screen_pos1, pos1, v_Normal[1], v_Color[1], v_Visible[1]);

    v_tri_dist = vec3(dist2, 0.0, 0.0);
    vertex(screen_pos2, pos2, v_Normal[2], v_Color[2], v_Visible[2]);

    EndPrimitive();
}