#version 400 core
#define PI 3.14159265359

layout(triangles, equal_spacing, ccw) in;

const int MAX_CASCADE_LEVEL = 8;

in vec3 tc_Pos[];
in vec3 tc_FaceNormal[];
in vec3 tc_VertexNormal[];
in vec4 tc_Color[];
flat in float tc_min_edge_length[];
in vec4 tc_rounding_sphere_center[];
in vec3 tc_center[];
in mat4 tc_LightSpacePos0[];
in mat4 tc_LightSpacePos1[];
// in float tc_clipspace_z[];
flat in int tc_Visible[];
flat in int tc_isTriangle[];
flat in float tc_VertexTypeRounded[];
flat in int tc_ovm_halfface_id[];

out vec3 v_pos;
out vec3 v_normal;
out vec4 v_color;
out vec2 v_uv;
out vec4 v_pos_ls[MAX_CASCADE_LEVEL];
out float v_clipspace_z;
flat out int v_visible;
flat out float v_VertexTypeRounded;
flat out int v_tes_inner_tri;
out float v_edge_factor;

out vec3 v_tri_dist;
flat out int v_is_triangle;
flat out vec4 v_a_adir;
flat out vec4 v_b_bdir;
flat out int v_use_lookup_path;


// necessary for calculating bezier mesh face normals
uniform mat4 u_transform;
uniform mat4 u_view;
uniform mat4 u_projection;

uniform mat4 u_light_projection[MAX_CASCADE_LEVEL];
uniform mat4 u_light_view[MAX_CASCADE_LEVEL];
uniform mat4 u_light_transform;
uniform float u_cell_size;
uniform bool u_draw_lines;
uniform bool u_rounding;
uniform float u_rounding_size;
uniform float u_average_cell_size;
uniform bool u_use_vertex_normals;

// uniforms for bezier meshes
uniform bool u_is_bezier_mesh;
uniform int u_bezier_degree;

// use texture buffer for control points
uniform samplerBuffer u_control_points_tb;

const float EDGE_FACTOR = 1.0 / sqrt(2.0);
const float CORNER_FACTOR = sqrt(2.0);

const ivec4 lookup[8] = ivec4[](
ivec4(3, 3, 3, 3), // [0][0][0]
ivec4(2, 2, 0, 1), // [0][0][1]
ivec4(1, 1, 0, 2), // [0][1][0]
ivec4(1, 2, 0, 0), // [0][1][1]
ivec4(0, 0, 1, 2), // [1][0][0]
ivec4(0, 2, 1, 1), // [1][0][1]
ivec4(0, 1, 2, 2), // [1][1][0]
ivec4(3, 3, 3, 3)// [1][1][1]
);

// a maximum bezier degree needs to be set because the De Casteljau algorithm
// needs an array and GLSL only supports local arrays with static sizes
#define MAX_BEZIER_DEGREE 7
#define MAX_CPS_PER_TRI (MAX_BEZIER_DEGREE+1)*(MAX_BEZIER_DEGREE+2)/2

#define CP_2D_INDEX_TO_1D(i2, i1, m) (((m)+1)*((m)+2)/2 - ((m)-(i2)+1)*((m)-(i2)+2)/2 + (i1))

vec3 control_points[MAX_CPS_PER_TRI];


int cp_2d_index_to_1d(int i2, int i1, int m)
{
    return (((m)+1)*((m)+2)/2 - ((m)-(i2)+1)*((m)-(i2)+2)/2 + (i1));
}

void de_casteljau_2(inout vec3 control_points[MAX_CPS_PER_TRI], int m, float x, float y, float z, int steps)
{
    // Analogous to the De Casteljau algorithm for bezier curves evaluate the 
    // bezier triangle of the current face.
    int cur_m = m;
    for (int s = m; s > m-steps; s--)
    {
        for (int i2 = 0; i2 < cur_m; i2++)
        {
            for (int i1 = 0; i1 < cur_m-i2; i1++)
            {
                int next_i = cp_2d_index_to_1d(i2, i1, cur_m-1);

                int i_1D = cp_2d_index_to_1d(i2, i1, cur_m);
                control_points[next_i] =  x*control_points[i_1D];

                i_1D = cp_2d_index_to_1d(i2+1, i1, cur_m);
                control_points[next_i] += y*control_points[i_1D];

                i_1D = cp_2d_index_to_1d(i2, i1+1, cur_m);
                control_points[next_i] += z*control_points[i_1D];
            }
        }
        cur_m--;
    }
}

void copy_control_points(inout vec3 dest[MAX_CPS_PER_TRI], int control_points_offset, int m)
{
    // Copy control points into dest which holds the temporary values for the
    // de Casteljau algorithm.
    int num_tri_cps = (m+2)*(m+1)/2;
    for (int i = 0; i < num_tri_cps; i++) {
        dest[i] = texelFetch(u_control_points_tb, control_points_offset + i).xyz;
    }
}

void set_light_space_pos(int vertex_index)
{
    v_pos_ls[0] = tc_LightSpacePos0[vertex_index][0];
    v_pos_ls[1] = tc_LightSpacePos0[vertex_index][1];
    v_pos_ls[2] = tc_LightSpacePos0[vertex_index][2];
    v_pos_ls[3] = tc_LightSpacePos0[vertex_index][3];
    v_pos_ls[4] = tc_LightSpacePos1[vertex_index][0];
    v_pos_ls[5] = tc_LightSpacePos1[vertex_index][1];
    v_pos_ls[6] = tc_LightSpacePos1[vertex_index][2];
    v_pos_ls[7] = tc_LightSpacePos1[vertex_index][3];
}

float get_shrink_factor(float angle, float dist) {
    float half_angle = angle * 0.5;
    return dist * (1.0 / cos(half_angle) - tan(half_angle));
}

float dist_to_edge(vec3 a, vec3 b, vec3 p)
{
    float h = min(1.0, max(0.0, dot(p-a, b-a) / dot(b-a, b-a)));
    return length(p-a-(b-a)*h);
}

float get_area(vec3 a, vec3 b, vec3 c)
{
    float l_a = length(a - c);
    float l_b = length(a - b);
    float l_c = length(b - c);
    float s = (l_a + l_b + l_c) * 0.5;
    return sqrt(s * (s - l_a) * (s - l_b) * (s - l_c));
}

int get_closest_corner_index(vec3 coords)
{
    return coords.x > coords.y ? coords.x > coords.z ? 0 : 2 : coords.y > coords.z ? 1 : 2;
}

void main()
{
    // barycentric coordinates of current triangle
    float x = gl_TessCoord.x;
    float y = gl_TessCoord.y;
    float z = gl_TessCoord.z;

    if (u_is_bezier_mesh)
    {
        // if the mesh is a bezier mesh evaluate the bezier triangle of the
        // current face for the given barycentric coordinates (x, y, z)


        // m is the bezier mesh degree (often used in papers)
        int m = u_bezier_degree;
        int ovm_hf_id = tc_ovm_halfface_id[0];
        int control_points_offset = (ovm_hf_id/2)*(m+2)*(m+1)/2;

        // copy control points from texture buffer into local array
        // so that it can be modified by the de casteljau algorithm
        copy_control_points(control_points, control_points_offset, m);

        // execute m-1 steps of the de casteljau algorithm
        de_casteljau_2(control_points, m, x, y, z, m-1);

        // Calculate normal of point on bézier triangle.
        // The three points generated by the decasteljau algorithm are the
        // tangent of the point of the bézier triangle at (x,y,z).
        // The normal of this tangent triangle is the normal of the point.
        vec3 a = control_points[cp_2d_index_to_1d(0, 0, 1)];
        vec3 b = control_points[cp_2d_index_to_1d(0, 1, 1)];
        vec3 c = control_points[cp_2d_index_to_1d(1, 0, 1)];
        v_normal = mat3(transpose(inverse(u_transform))) * cross(c - a, b - a);

        // Obtain the final position by doing the last step of linear interpolation
        // of the de casteljau algorithm.
        v_pos  = control_points[cp_2d_index_to_1d(0, 0, 1)]*x;
        v_pos += control_points[cp_2d_index_to_1d(1, 0, 1)]*y;
        v_pos += control_points[cp_2d_index_to_1d(0, 1, 1)]*z;

        // Perform Cell sizing.
        vec3 pos = tc_center[0] + (v_pos - tc_center[0]) * u_cell_size;

        v_pos = (u_transform * vec4(pos, 1.0)).xyz;
        vec4 screen_pos = u_projection * u_view * vec4(v_pos, 1.0);
        gl_Position = screen_pos;
        v_clipspace_z = screen_pos.z;

        // Do not render inner tessellated triangled in wireframe mode.
        // For a inner triangle no barycentric coordinate is 0.
        v_tes_inner_tri = int(ceil(min(min(x, y), z)));

        // Casced Shaowmap for Bézier meshes.
        // Cascaded Shadowmap (loops do not work here, we need to unroll the loop to compile this)
        mat4 light_space_mat = u_light_projection[0] * u_light_view[0] * u_light_transform;
        v_pos_ls[0] = light_space_mat * vec4(pos, 1.0);
        light_space_mat = u_light_projection[1] * u_light_view[1] * u_light_transform;
        v_pos_ls[1] = light_space_mat * vec4(pos, 1.0);
        light_space_mat = u_light_projection[2] * u_light_view[2] * u_light_transform;
        v_pos_ls[2] = light_space_mat * vec4(pos, 1.0);
        light_space_mat = u_light_projection[3] * u_light_view[3] * u_light_transform;
        v_pos_ls[3] = light_space_mat * vec4(pos, 1.0);
        light_space_mat = u_light_projection[4] * u_light_view[4] * u_light_transform;
        v_pos_ls[4] = light_space_mat * vec4(pos, 1.0);
        light_space_mat = u_light_projection[5] * u_light_view[5] * u_light_transform;
        v_pos_ls[5] = light_space_mat * vec4(pos, 1.0);
        light_space_mat = u_light_projection[6] * u_light_view[6] * u_light_transform;
        v_pos_ls[6] = light_space_mat * vec4(pos, 1.0);
        light_space_mat = u_light_projection[7] * u_light_view[7] * u_light_transform;
        v_pos_ls[7] = light_space_mat * vec4(pos, 1.0);

    }
    else
    {
        v_pos    = tc_Pos[0]    * x + tc_Pos[1]    * y + tc_Pos[2]    * z;
        v_tes_inner_tri = 0;

        if (u_use_vertex_normals)
        {
            v_normal = tc_VertexNormal[0] * x + tc_VertexNormal[1] * y + tc_VertexNormal[2] * z;
        }
        else
        {
            v_normal = tc_FaceNormal[0] * x + tc_FaceNormal[1] * y + tc_FaceNormal[2] * z;
        }

        vec4 screen_pos = u_projection * u_view * vec4(v_pos, 1.0);
        gl_Position = screen_pos;
        v_clipspace_z = screen_pos.z;
    }

    if (u_draw_lines)
    {

        vec3 pos0 = tc_Pos[0];
        vec3 pos1 = tc_Pos[1];
        vec3 pos2 = tc_Pos[2];

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

        vec4 tesselated_screen_pos = u_projection * u_view * vec4(tc_Pos[0] * x + tc_Pos[1] * y + tc_Pos[2] * z, 1.0);
        vec3 tesselated_ndc = tesselated_screen_pos.xyz / tesselated_screen_pos.w;

        float distX = dist_to_edge(ndc_pos[0], ndc_pos[1], tesselated_ndc);
        float distY = dist_to_edge(ndc_pos[1], ndc_pos[2], tesselated_ndc);
        float distZ = dist_to_edge(ndc_pos[0], ndc_pos[2], tesselated_ndc);

        float area = max(0.001, get_area(ndc_pos[0], ndc_pos[1], ndc_pos[2])) * 10.0;

        v_tri_dist = vec3(distX, distY, distZ);

        if (u_is_bezier_mesh || u_rounding)
        {
            v_tri_dist = v_tri_dist / area;
        }

        v_edge_factor = 0.0;
        if (x == 0.0 || y == 0.0 || z == 0.0)
        {
            v_edge_factor = 1.0;
        }

    }

    if (!u_is_bezier_mesh)
    {
        if (x > 0.0)
        {
            set_light_space_pos(0);
        }
        else if (y > 0.0)
        {
            set_light_space_pos(1);

        }
        else if (z > 0.0)
        {
            set_light_space_pos(2);
        }
    }


    // in any case use the values of vertex shader for these values 
    v_color           = tc_Color[0] * x + tc_Color[1] * y + tc_Color[2] * z;
    v_visible         = tc_Visible[1];// flat
    v_is_triangle      = tc_isTriangle[1];// flat
    v_VertexTypeRounded=tc_VertexTypeRounded[0] * x + tc_VertexTypeRounded[1] * y + tc_VertexTypeRounded[2] * z;
}