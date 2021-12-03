#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_offset;

uniform mat4 u_mesh_transform;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform vec3 u_cam_pos;

flat out int v_discard;

void main()
{
    vec3 view_dir = normalize(a_offset - u_cam_pos);
    vec3 normal = mat3(transpose(inverse(u_mesh_transform))) * a_normal;

    // currently, no spheres are discarded, but we could discard those whose vertex is invisible
    v_discard = 0;
    //v_discard = dot(view_dir, normal) > 0.01 ? 1 : 0;

    vec4 view_pos = u_view * u_mesh_transform * vec4(a_offset, 1.0);
    gl_Position = u_projection * (view_pos + vec4(a_pos * -view_pos.z * 0.02, 0.0));
}