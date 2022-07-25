#version 330 core

#define FLT_MAX 3.402823466e+38
const int MAX_CASCADE_LEVEL = 8;

layout (triangles) in;
layout (triangle_strip, max_vertices = 8) out;

// [P0.z > 0][P1.z > 0][P2.z > 0]
const ivec4 lookup[8] = ivec4[](
    ivec4(3, 3, 3, 3),  // [0][0][0]
    ivec4(2, 2, 0, 1),  // [0][0][1]
    ivec4(1, 1, 0, 2),  // [0][1][0]
    ivec4(1, 2, 0, 0),  // [0][1][1]
    ivec4(0, 0, 1, 2),  // [1][0][0]
    ivec4(0, 2, 1, 1),  // [1][0][1]
    ivec4(0, 1, 2, 2),  // [1][1][0]
    ivec4(3, 3, 3, 3)   // [1][1][1]
);

in vec3 v_Pos[3];
in vec3 v_Normal[3];
in vec4 v_Color[3];
in mat4 v_LightSpacePos0[3];
in mat4 v_LightSpacePos1[3];
flat in int v_Visible[3];

uniform mat4 u_transform;
uniform mat4 u_projection;
uniform mat4 u_view;

out vec3 v_pos;
out vec3 v_normal;
out vec4 v_pos_ls[MAX_CASCADE_LEVEL];
flat out int v_visible;
out float v_clipspace_z;


void set_light_space_pos(int vertex_index)
{
    v_pos_ls[0] = v_LightSpacePos0[vertex_index][0];
    v_pos_ls[1] = v_LightSpacePos0[vertex_index][1];
    v_pos_ls[2] = v_LightSpacePos0[vertex_index][2];
    v_pos_ls[3] = v_LightSpacePos0[vertex_index][3];
    v_pos_ls[4] = v_LightSpacePos1[vertex_index][0];
    v_pos_ls[5] = v_LightSpacePos1[vertex_index][1];
    v_pos_ls[6] = v_LightSpacePos1[vertex_index][2];
    v_pos_ls[7] = v_LightSpacePos1[vertex_index][3];
}

void vertex(vec4 screen_pos, vec3 pos, vec3 normal)
{
    gl_Position = screen_pos;
    v_pos = pos;
    v_normal = normal;
    v_clipspace_z = screen_pos.z;
    EmitVertex();
}

void main()
{
    vec3 pos0 = v_Pos[0];
    vec3 pos1 = v_Pos[1];
    vec3 pos2 = v_Pos[2];

    vec4 viewspace_pos0 = u_view * vec4(pos0, 1.0);
    vec4 viewspace_pos1 = u_view * vec4(pos1, 1.0);
    vec4 viewspace_pos2 = u_view * vec4(pos2, 1.0);

    vec4 screen_pos0 = u_projection * u_view * vec4(pos0, 1.0);
    vec4 screen_pos1 = u_projection * u_view * vec4(pos1, 1.0);
    vec4 screen_pos2 = u_projection * u_view * vec4(pos2, 1.0);


    set_light_space_pos(0);
    vertex(screen_pos0, viewspace_pos0.xyz, v_Normal[0]);

    set_light_space_pos(1);
    vertex(screen_pos1, viewspace_pos1.xyz, v_Normal[1]);

    set_light_space_pos(2);
    vertex(screen_pos2, viewspace_pos2.xyz, v_Normal[2]);

    EndPrimitive();
}