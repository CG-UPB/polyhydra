#version 400 core

#include "pbr.glsl"
#include "phong.glsl"

layout (location = 0) out vec4 FragColor;

in vec3 v_pos;
in vec3 v_normal;
in vec4 v_color;
flat in int v_visible;

uniform vec3 u_light_pos;
uniform vec3 u_cam_pos;
uniform vec3 u_light_color;
uniform vec4 u_object_color;
uniform int u_viewport_width;
uniform int u_viewport_height;

uniform float u_alpha_pow;
uniform float u_pow;
uniform float u_range;
uniform float u_depth_range;
uniform float u_ordering_strenth;
uniform float u_t_min;
uniform float u_t_max;

uniform bool  u_use_base_color;
uniform bool  u_two_sided_lighting;

uniform int u_current_layer;

// uniforms for bezier meshes
uniform bool u_is_bezier_mesh;

uniform sampler2D last_depth_texture;
uniform sampler2D max_depth_texture;

void main()
{
    vec4 used_color = vec4(1.0f);
    used_color = u_use_base_color ? u_object_color : v_color;

    vec3 n = normalize(v_normal);
    vec3 l = normalize(u_light_pos);
    vec3 v = normalize(u_cam_pos - v_pos);
    if((u_two_sided_lighting || u_is_bezier_mesh) && dot(n, v) <= 0 )
    {
        n = -n;
    }

    float frag_depth = gl_FragCoord.z;

    float last_depth = texelFetch(last_depth_texture, ivec2(gl_FragCoord.xy), 0).r;
    float max_depth = texelFetch(max_depth_texture, ivec2(gl_FragCoord.xy), 0).r;

    if((u_current_layer != 0 && frag_depth <= last_depth) || frag_depth >= max_depth || v_color.a >= 1.0 - 0.00001|| v_color.a == 0.0 || v_visible == 0)
    {
        discard;
    }
    vec3 result;
    if (u_use_pbr)
    {
        result = calculate_pbr_lighting(used_color.rgb, n, l, v, 1.0f, 0.0f, u_light_color);
    }
    else
    {
        result = calculate_phong_lighting(used_color.rgb, n, l, v, 1.0f, 0.0f, u_light_color);
    }

    FragColor = vec4(result.rgb, v_color.a);
}
