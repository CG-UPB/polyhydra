#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_center;
layout (location = 3) in float a_peel_depth;
layout (location = 4) in float a_is_boundary;


uniform mat4 u_transform;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform float u_cell_size;

uniform int u_peel_depth;
uniform float u_slice_depth;
uniform vec3 u_min;
uniform vec3 u_max;
uniform vec3 u_slice_direction;
uniform bool u_slice_locked;

out vec3 v_Normal;
flat out int v_Visible;

void main()
{
    ////////////////////////////////////////////////////////
    // Slicing and Peeling
    ////////////////////////////////////////////////////////
    v_Visible = 1;

    vec3 min = vec3(u_transform * vec4(u_min, 1.0));
    vec3 max = vec3(u_transform * vec4(u_max, 1.0));

    vec4 temp_dir = vec4(normalize(u_slice_direction), 0.0);
    if (u_slice_locked)
    {
        temp_dir = u_transform * temp_dir;
    }

    vec3 slice_dir = temp_dir.xyz;

    vec3 slice_point = max + u_slice_depth * (min - max);
    vec3 dir = slice_dir;
    vec3 center =  vec3(u_transform * vec4(a_center, 1.0));
    float angle = dot(normalize(dir), normalize(center - slice_point));

    if (a_peel_depth < u_peel_depth || angle > 0)
    {
        v_Visible = 0;
    }
    ////////////////////////////////////////////////////////

    vec3 pos = a_center + (a_pos - a_center) * u_cell_size;
    v_Normal = a_normal;
    gl_Position = u_projection * u_view * u_transform * vec4(pos, 1.0);
}