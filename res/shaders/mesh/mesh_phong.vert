#version 330 core

const int MAX_CASCADE_LEVEL = 8;
const int HALF_MAX_CASCADE_LEVEL = MAX_CASCADE_LEVEL / 2;

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_center;
layout (location = 3) in float a_peel_depth;
layout (location = 4) in float a_is_digged;
layout (location = 5) in vec4 a_color;
layout (location = 6) in float a_is_isolated;
layout (location = 7) in float a_is_triangle;
layout (location = 8) in float a_vertex_type_rounded;
layout (location = 9) in vec4 a_face_center_rounded;
layout (location = 10) in vec3 a_to_vertex_rounded;
layout (location = 11) in float a_dihedral_angle_rounded;
layout (location = 12) in float a_is_selected;
layout (location = 13) in float a_hovered;
layout (location = 14) in vec3 a_vertex_normal;
layout (location = 15) in float a_min_edge_length;

out vec3 v_Pos;
out vec3 v_Normal;
out vec4 v_Color;
flat out float v_min_edge_length;
out vec3 v_center;
out mat4 v_LightSpacePos0;
out mat4 v_LightSpacePos1;
// out float v_clipspace_z;
flat out int v_Visible;
flat out int v_isTriangle;
flat out float v_VertexTypeRounded;
flat out int v_ovm_halfface_id;
out vec4 v_S_c;

uniform mat4 u_transform;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_light_projection[MAX_CASCADE_LEVEL];
uniform mat4 u_light_view[MAX_CASCADE_LEVEL];
uniform mat4 u_light_transform;

uniform vec3 u_light_pos;
uniform vec3 u_cam_pos;
uniform vec3 u_light_color;
uniform vec4 u_object_color;
uniform float u_cell_size;
uniform vec4 u_selection_color;
uniform float u_average_cell_size;
uniform float u_peel_depth;
uniform float u_max_peel_depth;
uniform bool u_reverse_peeling;
uniform float u_slice_depth;
uniform vec3 u_min;
uniform vec3 u_max;
uniform vec3 u_slice_direction;
uniform bool u_slice_locked;
uniform bool u_rounding;
uniform float u_rounding_size;
uniform bool u_use_vertex_normals;
uniform int u_cascade_level;

// uniforms for bezier meshes
uniform bool u_is_bezier_mesh;


const float ROUNDED_VERTEX_TYPE_FACE     = 0.0;
const float ROUNDED_VERTEX_TYPE_EDGE     = 1.0;
const float ROUNDED_VERTEX_TYPE_CORNER   = 2.0;
const float ROUNDED_VERTEX_TYPE_CENTER   = 3.0;

const float EDGE_FACTOR = 1.0 / sqrt(2.0);
const float CORNER_FACTOR = sqrt(2.0);

float get_shrink_factor(float angle, float dist) {
    float half_angle = angle * 0.5;
    return dist * (1.0 / cos(half_angle) - tan(half_angle));
}

void main()
{
    v_center = a_center;
    v_S_c = a_face_center_rounded;

    // Use a_is_triangle as the ovm halfface id for bézier meshes.
    if(u_is_bezier_mesh)
    {
        v_ovm_halfface_id = int(a_is_triangle+0.5);
        v_isTriangle = 0;
    }
    else
    {
        v_ovm_halfface_id = -1;
    }

    // Visibility
    v_Visible = 1;

    mat4 view_transform = u_view * u_transform;

    vec3 min_slice = vec3(view_transform * vec4(u_min, 1.0));
    vec3 max_slice = vec3(view_transform * vec4(u_max, 1.0));
    vec3 slice_dir = (view_transform * vec4(normalize(u_slice_direction), 0.0)).xyz;
    vec3 slice_point = max_slice + u_slice_depth * (min_slice - max_slice);
    vec3 center = vec3(view_transform * vec4(a_center, 1.0));
    float angle = dot(normalize(slice_dir), normalize(center - slice_point));
    float peel_depth = a_peel_depth;

    if(u_reverse_peeling)
    {
        peel_depth = u_max_peel_depth - peel_depth;
    }

    // criteria for beeing invisible:
    // peeled, sliced, isolated or digged
    if (peel_depth + 1.0 <= u_peel_depth || angle > 0.0 || a_is_isolated == 1.0 || a_is_digged == 1.0)
    {
        v_Pos = vec3(0.0, 0.0, 0.0);
        v_Normal = vec3(0.0, 0.0, 0.0);
        v_Color = vec4(0.0, 0.0, 0.0, 0.0);
        v_min_edge_length = 0.0;
        v_Visible = 0;
        v_isTriangle = (a_is_triangle == 0.0) ? 0 : 1;
        return;
    }
    
    // Roundings
    vec3 position = a_pos;
    float alpha = u_object_color.a * a_color.a;
//    if (u_rounding)
//    {
//        float type = a_vertex_type_rounded;
//        float r = min(u_rounding_size * u_average_cell_size * 0.3, a_min_edge_length * 0.3);
//        // this vertex lies on the inner triangle
//        if (type == ROUNDED_VERTEX_TYPE_FACE)
//        {
//            position += normalize(a_face_center_rounded - position) * r;
//        }
//        // this vertex lies on an edge
//        else if (type == ROUNDED_VERTEX_TYPE_EDGE)
//        {
//            float dist = EDGE_FACTOR * r;
//            vec3 edge_dir = normalize(a_to_vertex_rounded - position);
//            vec3 shrink_dir = normalize(a_face_center_rounded - position);
//            position += edge_dir * dist + shrink_dir * get_shrink_factor(a_dihedral_angle_rounded, dist);
//        }
//        // this is a corner vertex
//        else if (type == ROUNDED_VERTEX_TYPE_CORNER)
//        {
//            float dist = CORNER_FACTOR * r;
//            vec3 shrink_dir = normalize(a_face_center_rounded - position);
//            position += shrink_dir * get_shrink_factor(a_dihedral_angle_rounded, dist);
//        }
//        v_VertexTypeRounded = type;
//    }

    mat4 cam_space_mat = u_projection * view_transform;
    vec3 pos = a_center + (position - a_center) * u_cell_size;

    vec3 normal = a_normal;
    if (u_use_vertex_normals)
    {
        normal = a_vertex_normal;
    }

    v_Pos = vec3(u_transform * vec4(pos, 1.0));
    v_Normal = mat3(transpose(inverse(u_transform))) * normal;

    // Cascaded Shadowmap (loops do not work here, we need to unroll the loop to compile this)
    mat4 light_space_mat = u_light_projection[0] * u_light_view[0] * u_light_transform;
    v_LightSpacePos0[0] = light_space_mat * vec4(pos, 1.0);
    light_space_mat = u_light_projection[1] * u_light_view[1] * u_light_transform;
    v_LightSpacePos0[1] = light_space_mat * vec4(pos, 1.0);
    light_space_mat = u_light_projection[2] * u_light_view[2] * u_light_transform;
    v_LightSpacePos0[2] = light_space_mat * vec4(pos, 1.0);
    light_space_mat = u_light_projection[3] * u_light_view[3] * u_light_transform;
    v_LightSpacePos0[3] = light_space_mat * vec4(pos, 1.0);
    light_space_mat = u_light_projection[4] * u_light_view[4] * u_light_transform;
    v_LightSpacePos1[0] = light_space_mat * vec4(pos, 1.0);
    light_space_mat = u_light_projection[5] * u_light_view[5] * u_light_transform;
    v_LightSpacePos1[1] = light_space_mat * vec4(pos, 1.0);
    light_space_mat = u_light_projection[6] * u_light_view[6] * u_light_transform;
    v_LightSpacePos1[2] = light_space_mat * vec4(pos, 1.0);
    light_space_mat = u_light_projection[7] * u_light_view[7] * u_light_transform;
    v_LightSpacePos1[3] = light_space_mat * vec4(pos, 1.0);

    v_isTriangle = (a_is_triangle == 0.0) ? 0 : 1;
    v_min_edge_length =  a_min_edge_length;
    v_center = a_center;

    float peel_alpha = (u_peel_depth - peel_depth);
    if(v_Visible == 1 && peel_alpha < 1.0 && peel_alpha > 0.0)
    {
        alpha = (1.0 - (u_peel_depth - peel_depth)) * alpha;
    }

    v_Color = vec4(mix(u_object_color.rgb, a_color.rgb, a_color.a), alpha);

    if (a_hovered != 0.0)
    {
        float hover_strength = 0.5;
        vec3 hover_color = vec3(0.9, 0.1, 0.1);
        v_Color = vec4(mix(v_Color.rgb, hover_color, 0.3), 1.0) + vec4(hover_color * hover_strength, 0.0);
        v_Color.a = alpha;
    }
}