#version 330 core

const vec3 CYLINDER_UP = vec3(0.0, 1.0, 0.0);

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_from_vertex;
layout (location = 2) in vec3 a_to_vertex;

uniform mat4 u_mesh_transform;
uniform mat4 u_projection;
uniform mat4 u_view;

flat out int v_instance_id;

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
    v_instance_id = gl_InstanceID;

    float edge_length = length(a_to_vertex - a_from_vertex);
    vec3 edge_dir = normalize(a_to_vertex - a_from_vertex);
    float rot_angle = acos(dot(CYLINDER_UP, edge_dir));
    vec3 rot_axis = normalize(cross(CYLINDER_UP, edge_dir));
    mat4 rotation = inverse(get_rotation_matrix(rot_axis, rot_angle));

    vec3 offset = a_from_vertex + (a_to_vertex - a_from_vertex) * 0.5;
    float width = 0.07 * (1.0 / length(u_mesh_transform[0]));
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