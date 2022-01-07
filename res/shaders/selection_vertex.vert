#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_offset;
layout (location = 3) in vec3 a_center;

uniform mat4 u_mesh_transform;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform vec3 u_cam_pos;
uniform float u_cell_size;

flat out int v_discard;
flat out int v_instance_id;

void main()
{
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