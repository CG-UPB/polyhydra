#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_position;
layout (location = 3) in vec3 a_scale;
layout (location = 4) in vec4 a_rotation;
layout (location = 5) in vec3 a_color;

layout (location = 6) in float a_has_cell;
layout (location = 7) in vec3 a_cell_center;
layout (location = 8) in float a_cell_peel_depth;
layout (location = 9) in float a_is_dug;
layout (location = 10) in float a_is_isolated;

uniform mat4 u_mesh_transform;
uniform mat4 u_projection;
uniform mat4 u_view;

uniform vec3 u_min;
uniform vec3 u_max;
uniform vec3 u_slice_direction;
uniform float u_slice_depth;
uniform float u_peel_depth;
uniform float u_cell_size;
uniform float u_scale_normalization;

out vec3 v_pos;
out vec3 v_normal;
out vec3 v_color;
flat out int v_visible;

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

mat4 look_at(vec3 direction, vec3 up)
{
    if(dot(direction, up) == 1.0)
    {
        float tmp = direction.x;
        if(direction.y >= tmp)
            tmp = direction.x;
        if(direction.z >= tmp)
            tmp = direction.z;

        direction.x += 0.000001 * tmp;
        direction.y += 0.000001 * tmp;
        direction.z += 0.000001 * tmp;
    }

    vec3 y_axis = normalize(direction);
    vec3 x_axis = normalize(cross(up, y_axis));
    vec3 z_axis = cross(y_axis, x_axis);
    return mat4(
        x_axis.x, y_axis.x, z_axis.x, 0.0,
        x_axis.y, y_axis.y, z_axis.y, 0.0,
        x_axis.z, y_axis.z, z_axis.z, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

void main()
{
    v_visible = 1;
    vec3 position = a_position;
    float scale_offset = 1.0;
    if (a_has_cell == 1.0)
    {
        mat4 view_transform = u_view * u_mesh_transform;

        vec3 min_slice = vec3(view_transform * vec4(u_min, 1.0));
        vec3 max_slice = vec3(view_transform * vec4(u_max, 1.0));
        vec3 slice_dir = (view_transform * vec4(normalize(u_slice_direction), 0.0)).xyz;
        vec3 slice_point = max_slice + u_slice_depth * (min_slice - max_slice);
        vec3 center = vec3(view_transform * vec4(a_cell_center, 1.0));
        float angle = dot(normalize(slice_dir), normalize(center - slice_point));

        if (a_cell_peel_depth < u_peel_depth || angle > 0.0 || a_is_isolated == 1.0 || a_is_dug == 1.0)
        {
            v_visible = 0;
            v_pos = vec3(0.0, 0.0, 0.0);
            v_normal = vec3(0.0, 0.0, 0.0);
            v_color = vec3(0.0, 0.0, 0.0);
            return;
        }
        position = a_cell_center + (position - a_cell_center) * u_cell_size;
        scale_offset = u_cell_size;
    }
    //scale_offset /= u_scale_normalization;
    mat4 translation = mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        position.x, position.y, position.z, 1.0
    );
    mat4 scale = mat4(
        a_scale.x * scale_offset, 0.0, 0.0, 0.0,
        0.0, a_scale.y * scale_offset, 0.0, 0.0,
        0.0, 0.0, a_scale.z * scale_offset, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
    mat4 rotation = look_at(a_rotation.xyz, vec3(cos(a_rotation.w), 0.0, sin(a_rotation.w)));
    mat4 transform = translation * inverse(rotation) * scale;

    v_pos = vec3(u_mesh_transform * transform * vec4(a_pos, 1.0));
    v_normal = mat3(transpose(inverse(u_mesh_transform * transform))) * a_normal;
    v_color = a_color;
    gl_Position = u_projection * u_view * u_mesh_transform * transform * vec4(a_pos, 1.0);
}