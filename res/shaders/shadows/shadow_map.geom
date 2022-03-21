#version 330 core

#define FLT_MAX 3.402823466e+38

layout (triangles) in;
layout (triangle_strip, max_vertices = 8) out;

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

flat in int v_Visible[3];
flat in int v_isTriangle[3];

uniform mat4 u_light_projection;
uniform mat4 u_light_view;

out vec3 v_pos;
flat out int v_visible;
out vec3 v_tri_dist;
flat out int v_is_triangle;
flat out vec4 v_a_adir;
flat out vec4 v_b_bdir;
flat out int v_use_lookup_path;

float dist_to_edge(vec3 e0, vec3 e1, vec3 p)
{
    return length(cross(p - e0, p - e1)) / length(e1 - e0);
}

void vertex(vec4 screen_pos, vec3 pos, int visible)
{
    gl_Position = screen_pos;
    v_pos = pos;
    v_visible = visible;
    EmitVertex();
}

void main()
{
    vec3 pos0 = v_Pos[0];
    vec3 pos1 = v_Pos[1];
    vec3 pos2 = v_Pos[2];

    vec4 viewspace_pos0 = u_light_view * vec4(pos0, 1.0);
    vec4 viewspace_pos1 = u_light_view * vec4(pos1, 1.0);
    vec4 viewspace_pos2 = u_light_view * vec4(pos2, 1.0);

    vec4 screen_pos0 = u_light_projection * viewspace_pos0;
    vec4 screen_pos1 = u_light_projection * viewspace_pos1;
    vec4 screen_pos2 = u_light_projection * viewspace_pos2;

    vec3 ndc_pos[3] = vec3[](
    vec3(screen_pos0.xyz / screen_pos0.w),
    vec3(screen_pos1.xyz / screen_pos1.w),
    vec3(screen_pos2.xyz / screen_pos2.w)
    );

    int lookup_case = 4 * int(ndc_pos[0].z > 0) + 2 * int(ndc_pos[1].z > 0) + int(ndc_pos[2].z > 0);
    ivec4 ndc_index = lookup[lookup_case];
    if (ndc_index.x < 3)
    {
        vec2 a_p = ndc_pos[ndc_index.x].xy;
        vec2 b_p = ndc_pos[ndc_index.y].xy;
        vec2 aa_p = ndc_pos[ndc_index.z].xy;
        vec2 bb_p = ndc_pos[ndc_index.w].xy;

        vec2 a_v = a_p.xy * 0.5 + 0.5;
        vec2 b_v = b_p.xy * 0.5 + 0.5;
        vec2 a_dir = normalize(a_v - ((a_p + (aa_p - a_p)) * 0.5 + 0.5));
        vec2 b_dir = normalize(b_v - ((b_p + (bb_p - b_p)) * 0.5 + 0.5));

        v_a_adir = vec4(a_v, a_dir);
        v_b_bdir = vec4(b_v, b_dir);
        v_use_lookup_path = 1;
    }
    else
    {
        v_use_lookup_path = 0;
    }

    v_is_triangle = v_isTriangle[0];

    float dist0 = dist_to_edge(ndc_pos[1], ndc_pos[2], ndc_pos[0]);
    float dist1 = dist_to_edge(ndc_pos[0], ndc_pos[2], ndc_pos[1]);
    float dist2 = dist_to_edge(ndc_pos[0], ndc_pos[1], ndc_pos[2]);

    v_tri_dist = vec3(0.0, dist0, 0.0);
    vertex(screen_pos0, pos0, v_Visible[0]);

    v_tri_dist = vec3(0.0, 0.0, dist1);
    vertex(screen_pos1, pos1, v_Visible[1]);

    v_tri_dist = vec3(dist2, 0.0, 0.0);
    vertex(screen_pos2, pos2, v_Visible[2]);

    EndPrimitive();
}