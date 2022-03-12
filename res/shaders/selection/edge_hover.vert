#version 330 core

const vec3 UP = vec3(0.0, 1.0, 0.0);

layout (location = 0) in vec3 a_pos;

uniform vec3 u_from_vertex;
uniform vec3 u_to_vertex;
uniform mat4 u_mesh_transform;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform float u_average_cell_size;

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
    float edge_length = length(u_to_vertex - u_from_vertex);
    vec3 edge_dir = normalize(u_to_vertex - u_from_vertex);
    float rot_angle = acos(dot(UP, edge_dir));
    vec3 rot_axis = normalize(cross(UP, edge_dir));
    mat4 rotation = inverse(get_rotation_matrix(rot_axis, rot_angle));

    vec3 offset = u_from_vertex + (u_to_vertex - u_from_vertex) * 0.5;
    vec4 normalization = vec4(1.0, 0.0, 0.0, 1.0);
    normalization = u_view * u_mesh_transform * normalization;
    float width = 0.1 * u_average_cell_size;
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