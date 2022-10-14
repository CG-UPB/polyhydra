#version 330 core

#define FLT_MAX 3.402823466e+38
const int MAX_CASCADE_LEVEL = 8;

layout (triangles) in;
layout (triangle_strip, max_vertices = 8) out;

const float ROUNDED_VERTEX_TYPE_FACE     = 0.0;
const float ROUNDED_VERTEX_TYPE_EDGE     = 1.0;
const float ROUNDED_VERTEX_TYPE_CORNER   = 2.0;
const float ROUNDED_VERTEX_TYPE_CENTER   = 3.0;

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
flat in int v_isTriangle[3];
flat in float v_VertexTypeRounded[3];

uniform mat4 u_transform;
uniform mat4 u_projection;
uniform mat4 u_view;

uniform bool u_rounding;

out vec3 v_pos;
out vec3 v_normal;
out vec4 v_color;
out vec4 v_pos_ls[MAX_CASCADE_LEVEL];
flat out int v_visible;
out float v_clipspace_z;

out vec3 v_tri_dist;
flat out int v_is_triangle;
flat out vec4 v_a_adir;
flat out vec4 v_b_bdir;
flat out int v_use_lookup_path;

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
    v_clipspace_z = screen_pos.z;
    EmitVertex();
}

// when rounding is active, the color of the added rounding vertices should match the adjacent faces
vec4 get_color(int i0, int i1, int i2)
{
    float type = v_VertexTypeRounded[i0];
    if (u_rounding && (type == ROUNDED_VERTEX_TYPE_EDGE || type == ROUNDED_VERTEX_TYPE_CORNER))
    {
        int faceIndex = v_VertexTypeRounded[i1] == ROUNDED_VERTEX_TYPE_FACE ? i1 : i2;
        return v_Color[faceIndex];
    }
    return v_Color[i0];
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

    float dist0 = dist_to_edge(ndc_pos[1], ndc_pos[2], ndc_pos[0]);
    float dist1 = dist_to_edge(ndc_pos[0], ndc_pos[2], ndc_pos[1]);
    float dist2 = dist_to_edge(ndc_pos[0], ndc_pos[1], ndc_pos[2]);

    // x is min -> p0 - p1 closest, y is min -> p1 - p2 closest, z is min -> p0 - p2 closest
    // !is_triangle && x or z is min -> discard
    v_is_triangle = v_isTriangle[0];

    set_light_space_pos(0);
    v_tri_dist = vec3(0.0, dist0, 0.0);
    vertex(screen_pos0, pos0.xyz, v_Normal[0], get_color(0, 1, 2), v_Visible[0]);

    set_light_space_pos(1);
    v_tri_dist = vec3(0.0, 0.0, dist1);
    vertex(screen_pos1, pos1.xyz, v_Normal[1], get_color(1, 0, 2), v_Visible[1]);

    set_light_space_pos(2);
    v_tri_dist = vec3(dist2, 0.0, 0.0);
    vertex(screen_pos2, pos2.xyz, v_Normal[2], get_color(2, 0, 1), v_Visible[2]);

    EndPrimitive();
}