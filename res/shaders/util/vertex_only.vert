#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_offset;
layout (location = 3) in vec3 a_center;
layout (location = 4) in float a_peel_depth;
layout (location = 5) in vec2 a_is_digged;
layout (location = 6) in float a_is_isolated;

uniform mat4 u_mesh_transform;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform vec3 u_cam_pos;
uniform float u_cell_size;
uniform float u_average_cell_size;
uniform float u_size;

uniform int u_peel_depth;
uniform float u_max_peel_depth;
uniform bool u_reverse_peeling;
uniform float u_slice_depth;
uniform vec3 u_min;
uniform vec3 u_max;
uniform vec3 u_slice_direction;
uniform bool u_slice_locked;

flat out int v_visible;
flat out int v_discard;

void main()
{
    ////////////////////////////////////////////////////////
    // Slicing and Peeling
    ////////////////////////////////////////////////////////
    v_visible = 1;

    float is_digged = a_is_digged[0];
    float never_discarad = a_is_digged[1];
    if (is_digged == 1.0 || a_is_isolated == 1.0)
    {
        v_visible = 0;
    }

    mat4 view_transform = u_view * u_mesh_transform;

    vec3 min_slice = vec3(view_transform * vec4(u_min, 1.0));
    vec3 max_slice = vec3(view_transform * vec4(u_max, 1.0));

    vec4 temp_dir = view_transform * vec4(normalize(u_slice_direction), 0.0);
    vec3 slice_dir = temp_dir.xyz;

    vec3 slice_point = max_slice + u_slice_depth * (min_slice - max_slice);
    vec3 dir = slice_dir;
    vec3 center = vec3(view_transform * vec4(a_center, 1.0));
    float angle = dot(normalize(dir), normalize(center - slice_point));
    float peel_depth = a_peel_depth;

    if(u_reverse_peeling)
    {
        peel_depth = u_max_peel_depth - peel_depth;
    }

    if (peel_depth < u_peel_depth || angle > 0)
    {
        v_visible = 0;
    }
    ////////////////////////////////////////////////////////

    // currently, no spheres are discarded, but we could discard those whose vertex is invisible
    v_discard = 0;
    //v_discard = dot(view_dir, normal) > 0.01 ? 1 : 0;

    vec3 offset = a_center + (a_offset - a_center) * u_cell_size;
    float width = 0.15 * u_average_cell_size * u_size;
    mat4 scale = mat4(
        width, 0.0, 0.0, 0.0,
        0.0, width, 0.0, 0.0,
        0.0, 0.0, width, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
    mat4 position = mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        offset.x, offset.y, offset.z, 1.0
    );
    mat4 transform = u_mesh_transform * mat4(position * scale);
    gl_Position = u_projection * u_view * transform * vec4(a_pos, 1.0);
}