#version 330 core

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Center;
layout (location = 3) in float a_peel_depth;
layout (location = 4) in float a_isBoundary;
layout (location = 5) in float a_isDigged;

out vec3 v_Pos;
out vec3 v_Normal;
flat out int v_Visible;

uniform mat4 u_Transform;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform vec3 u_lightPos;
uniform vec3 u_camPos;
uniform vec3 u_lightColor;
uniform vec3 u_objectColor;
uniform float u_cell_size;

uniform int u_peel_depth;
uniform float u_slice_depth;
uniform vec3 u_min;
uniform vec3 u_max;
uniform vec3 u_slice_direction;
uniform bool u_slice_locked;


void main()
{
    ////////////////////////////////////////////////////////
    // Slicing and Peeling
    ////////////////////////////////////////////////////////
    v_Visible = 1;

    if (a_isDigged == 0.0)
    {
        v_Visible = 0;
    }

    vec3 min = vec3(u_Transform * vec4(u_min, 1.0));
    vec3 max = vec3(u_Transform * vec4(u_max, 1.0));

    vec4 temp_dir = vec4(normalize(u_slice_direction), 0.0);
    if (u_slice_locked)
    {
        temp_dir = u_Transform * temp_dir;
    }

    vec3 slice_dir = temp_dir.xyz;

    vec3 slice_point = max + u_slice_depth * (min - max);
    vec3 dir = slice_dir;
    vec3 center =  vec3(u_Transform * vec4(a_Center, 1.0));
    float angle = dot(normalize(dir), normalize(center - slice_point));

    if (a_peel_depth < u_peel_depth || angle > 0)
    {
        v_Visible = 0;
    }
    ////////////////////////////////////////////////////////

    vec3 pos = a_Center + (a_Pos - a_Center) * u_cell_size;
    v_Pos = vec3(u_Transform * vec4(pos, 1.0));
    v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
}