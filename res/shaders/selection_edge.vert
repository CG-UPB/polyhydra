#version 330 core

const vec3 CYLINDER_UP = vec3(0.0, 1.0, 0.0);

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_from_vertex;
layout (location = 2) in vec3 a_to_vertex;
layout (location = 3) in vec3 a_center;
layout (location = 4) in float a_peelDepth;

uniform mat4 u_mesh_transform;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform float u_cell_size;

uniform int u_peelDepth;
uniform int u_sliceCoord;
uniform float u_sliceMin;

flat out int v_visible;

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
    v_visible = 1;
    float slicePos = (u_sliceCoord == 0) ? a_center.x : (u_sliceCoord == 1) ? a_center.y : (u_sliceCoord == 2) ? a_center.z : 0.0;
    if (a_peelDepth < u_peelDepth || slicePos < u_sliceMin)
    {
        v_visible = 0;
    }

    vec3 from = a_center + (a_from_vertex - a_center) * u_cell_size;
    vec3 to = a_center + (a_to_vertex - a_center) * u_cell_size;

    v_instance_id = gl_InstanceID;

    float edge_length = length(to - from);
    vec3 edge_dir = normalize(to - from);
    float rot_angle = acos(dot(CYLINDER_UP, edge_dir));
    vec3 rot_axis = normalize(cross(CYLINDER_UP, edge_dir));
    mat4 rotation = inverse(get_rotation_matrix(rot_axis, rot_angle));

    vec3 offset = from + (to - from) * 0.5;
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