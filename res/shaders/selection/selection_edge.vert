#version 330 core

const vec3 CYLINDER_UP = vec3(0.0, 1.0, 0.0);

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_from_vertex;
layout (location = 2) in vec3 a_to_vertex;
layout (location = 3) in vec3 a_center;
layout (location = 4) in float a_peel_depth;
layout (location = 5) in float a_is_digged;
layout (location = 6) in float a_is_isolated;

uniform mat4 u_mesh_transform;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform float u_cell_size;
uniform float u_average_cell_size;

uniform int u_peel_depth;
uniform float u_slice_depth;
uniform vec3 u_min;
uniform vec3 u_max;
uniform vec3 u_slice_direction;
uniform bool u_slice_locked;

flat out int v_visible;
flat out int v_edge_id;

mat4 get_rotation_matrix(vec3 axis, float angle)
{
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    return mat4(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0,
                oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s, 0.0,
                oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c, 0.0,
                0.0, 0.0, 0.0, 1.0);
}

void main()
{
    ////////////////////////////////////////////////////////
    // Slicing and Peeling
    ////////////////////////////////////////////////////////
    v_visible = 1;

    if (a_is_digged == 1.0 || a_is_isolated == 1.0)
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

    if (a_peel_depth < u_peel_depth || angle > 0)
    {
        v_visible = 0;
    }
    ////////////////////////////////////////////////////////

    vec3 from = a_center + (a_from_vertex - a_center) * u_cell_size;
    vec3 to = a_center + (a_to_vertex - a_center) * u_cell_size;

    v_edge_id = gl_InstanceID;

    float edge_length = length(to - from);
    vec3 edge_dir = normalize(to - from);
    float rot_angle = acos(dot(CYLINDER_UP, edge_dir));
    vec3 rot_axis = normalize(cross(CYLINDER_UP, edge_dir));
    mat4 rotation = inverse(get_rotation_matrix(rot_axis, rot_angle));

    vec3 offset = from + (to - from) * 0.5;
    vec4 normalization = vec4(1.0, 0.0, 0.0, 1.0);
    normalization = u_view * u_mesh_transform * normalization;
    float width = 0.15 * u_average_cell_size;
    mat4 scale = mat4(
        width, 0.0, 0.0, 0.0,
        0.0, edge_length, 0.0, 0.0,
        0.0, 0.0, width, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
    mat4 position = mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        offset.x, offset.y, offset.z, 1.0
    );
    mat4 transform = position * rotation * scale;
    gl_Position = u_projection * u_view * u_mesh_transform * transform * vec4(a_pos, 1.0);
}