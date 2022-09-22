#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_center;
layout (location = 3) in float a_peel_depth;
layout (location = 4) in float a_is_digged;
layout (location = 5) in vec4 a_color;
layout (location = 6) in float a_is_isolated;
layout (location = 7) in float a_is_triangle;
layout (location = 8) in float a_vertex_type_rounded;
layout (location = 9) in vec3 a_face_center_rounded;
layout (location = 10) in vec3 a_to_vertex_rounded;
layout (location = 11) in float a_dihedral_angle_rounded;
layout (location = 15) in float a_min_edge_length;

out vec3 v_normal;
flat out int v_visible;

uniform mat4 u_transform;
uniform mat4 u_projection;
uniform mat4 u_view;

uniform vec3 u_cam_pos;
uniform vec4 u_object_color;
uniform float u_cell_size;
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
    // Visibility
    v_visible = 1;

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
        v_visible = 0;
    }

    // Roundings
    vec3 position = a_pos;
    float alpha = u_object_color.a * a_color.a;
    if (u_rounding)
    {
        float type = a_vertex_type_rounded;
        float r = min(u_rounding_size * u_average_cell_size * 0.3, a_min_edge_length * 0.3);
        // this vertex lies on the inner triangle
        if (type == ROUNDED_VERTEX_TYPE_FACE)
        {
            position += normalize(a_face_center_rounded - position) * r;
        }
        // this vertex lies on an edge
        else if (type == ROUNDED_VERTEX_TYPE_EDGE)
        {
            float dist = EDGE_FACTOR * r;
            vec3 edge_dir = normalize(a_to_vertex_rounded - position);
            vec3 shrink_dir = normalize(a_face_center_rounded - position);
            position += edge_dir * dist + shrink_dir * get_shrink_factor(a_dihedral_angle_rounded, dist);
        }
        // this is a corner vertex
        else if (type == ROUNDED_VERTEX_TYPE_CORNER)
        {
            float dist = CORNER_FACTOR * r;
            vec3 shrink_dir = normalize(a_face_center_rounded - position);
            position += shrink_dir * get_shrink_factor(a_dihedral_angle_rounded, dist);
        }
    }

    v_normal = mat3(transpose(inverse(view_transform))) * a_normal;

    float peel_alpha = (u_peel_depth - peel_depth);
    if(v_visible == 1 && peel_alpha < 1.0 && peel_alpha > 0.0)
    {
        alpha = (1.0 - (u_peel_depth - peel_depth)) * alpha;
    }
    if (alpha < 1.0 - 0.01)
    {
        v_visible = 0;
    }
    vec3 pos = a_center + (position - a_center) * u_cell_size;
    gl_Position = u_projection * u_view * u_transform * vec4(pos, 1.0);
}