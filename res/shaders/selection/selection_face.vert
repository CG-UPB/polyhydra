#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_center;
layout (location = 3) in float a_peel_depth;
layout (location = 4) in float a_is_digged;
layout (location = 6) in float a_is_isolated;
// a_is_triangle holds the halfface id for Bézier meshes
layout (location = 7) in float a_is_triangle;

uniform mat4 u_mesh_transform;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform float u_cell_size;

uniform float u_peel_depth;
uniform float u_slice_depth;
uniform vec3 u_min;
uniform vec3 u_max;
uniform vec3 u_slice_direction;
uniform bool u_slice_locked;

// uniforms for bezier meshes
uniform bool u_is_bezier_mesh;

out vec4 v_Pos;
flat out int v_visible;
flat out int v_ovm_halfface_id;
flat out vec3 v_center;

void main()
{
    v_center = a_center;
    
    // Use a_is_triangle as the ovm halfface id for bézier meshes.
    if(u_is_bezier_mesh)
    {
        v_ovm_halfface_id = int(a_is_triangle+0.5);
    }
    else
    {
        v_ovm_halfface_id = -1;
    }

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

    vec3 pos = a_center + (a_pos - a_center) * u_cell_size;
    v_Pos = u_projection * u_view * u_mesh_transform * vec4(pos, 1.0);
}