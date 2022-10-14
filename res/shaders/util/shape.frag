#version 330 core

#include "pbr.glsl"
#include "phong.glsl"

in vec3 v_pos;
in vec3 v_normal;
in vec3 v_color;
flat in int v_visible;

uniform vec3 u_light_pos;
uniform vec3 u_cam_pos;
uniform vec3 u_light_color;

out vec4 fragment_color;

void main()
{
    if (v_visible == 0)
    {
        discard;
    }
    vec3 n = normalize(v_normal);
    vec3 l = normalize(u_light_pos);
    vec3 v = normalize(u_cam_pos - v_pos);
    if (u_use_pbr)
    {
        fragment_color = vec4(calculate_pbr_lighting(v_color.rgb, n, l, v, 1.0, 0.0, u_light_color), 1.0);
    }
    else
    {
        fragment_color = vec4(calculate_phong_lighting(v_color.rgb, n, l, v, 1.0, 0.0, u_light_color), 1.0);
    }
}