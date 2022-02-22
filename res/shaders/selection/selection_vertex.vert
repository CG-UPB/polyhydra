#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_offset;
layout (location = 3) in vec3 a_center;
layout (location = 4) in float a_peelDepth;
layout (location = 5) in float a_is_digged;
layout (location = 6) in float a_is_isolated;

uniform mat4 u_mesh_transform;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform vec3 u_cam_pos;
uniform float u_cell_size;

uniform int u_peel_depth;
uniform float u_slice_depth;
uniform vec3 u_min;
uniform vec3 u_max;
uniform vec3 u_slice_direction;
uniform bool u_slice_locked;

flat out int v_visible;
flat out int v_discard;
flat out int v_instance_id;

void main()
{
    ////////////////////////////////////////////////////////
    // Slicing and Peeling
    ////////////////////////////////////////////////////////
    v_visible = 1;

    vec3 min = vec3(u_mesh_transform * vec4(u_min, 1.0));
    vec3 max = vec3(u_mesh_transform * vec4(u_max, 1.0));

    vec4 temp_dir = vec4(normalize(u_slice_direction), 0.0);
    if (u_slice_locked)
    {
        temp_dir = u_mesh_transform * temp_dir;
    }

    vec3 slice_dir = temp_dir.xyz;

    vec3 slice_point = max + u_slice_depth * (min - max);
    vec3 dir = slice_dir;
    vec3 center =  vec3(u_mesh_transform * vec4(a_center, 1.0));
    float angle = dot(normalize(dir), normalize(center - slice_point));

    if (a_peelDepth < u_peel_depth || angle > 0 || a_is_digged == 0.0 || a_is_isolated == 0.0)
    {
        v_visible = 0;
    }
    ////////////////////////////////////////////////////////


    vec3 off = a_center + (a_offset - a_center) * u_cell_size;

    v_instance_id = gl_InstanceID;

    vec3 view_dir = normalize(off - u_cam_pos);
    vec3 normal = mat3(transpose(inverse(u_mesh_transform))) * a_normal;

    // currently, no spheres are discarded, but we could discard those whose vertex is invisible
    v_discard = 0;
    //v_discard = dot(view_dir, normal) > 0.01 ? 1 : 0;

    vec3 offset = off;
    float width = 0.2 * (1.0 / length(u_mesh_transform[0]));
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