#version 400 core

#include "pbr.glsl"
#include "phong.glsl"

layout (location = 0) out vec4 accum;
layout (location = 1) out float reveal;
layout (location = 2) out vec3 modulate;

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

// uniforms for bezier meshes
uniform bool u_is_bezier_mesh;

float near = 0.1f;
float far = 100.0f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    vec4 used_color = vec4(1.0f);
    used_color = u_use_base_color ? u_object_color : v_color;

    vec3 n = normalize(v_normal);
    // For Bézier meshes, always use a normal directed towards the camera.
    if(u_is_bezier_mesh && dot(n, normalize(u_cam_pos -  v_pos)) < 0)
    {
        n = -n;
    }

    vec3 l = normalize(u_light_pos);
    vec3 v = normalize(u_cam_pos - v_pos);
    if(u_two_sided_lighting && dot(n, v) <= 0 )
    {
        n = -n;
    }

    if(v_color.a >= 1.0 - 0.001 || v_visible == 0  || v_color.a == 0.0)
    {
        discard;
    }
    vec3 light_color = u_light_color;

    vec2 uv = gl_FragCoord.xy / vec2(u_viewport_width, u_viewport_height);

    vec3 result;
    if (u_use_pbr)
    {
        result = calculate_pbr_lighting(used_color.rgb, n, l, v, 1.0f, 0.0f, u_light_color);
    }
    else
    {
        result = calculate_phong_lighting(used_color.rgb, n, l, v, 1.0f, 0.0f, u_light_color);
    }
    used_color.rgb = result;

    float depth = gl_FragCoord.z;
    //float depth = LinearizeDepth(gl_FragCoord.z) / far;

    //color.a = pow(color.a, 2.0);

    // choose weight function
    float weight = clamp(pow(min(1.0, v_color.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - depth * 0.5, 3.0), 1e-2, 3e3);
    //float weight = pow(color.a, u_pow) * clamp(u_range / (1e-5 + pow(depth * 0.8, u_ordering_strenth)), u_t_min, u_t_max);
    //float weight = pow(color.a + 0.01, 4.0) + max(1e-2, min(3.0 * 1e3, 100.0 / (1e-5 + pow(abs(depth) / 10.0, 3.0) + pow(abs(depth) / 200.0, 6.0))));

    //float weight = 1.0 / pow(1.0 + LinearizeDepth(depth), u_pow);

    accum = vec4(used_color.rgb * v_color.a, v_color.a) * weight;
    reveal = v_color.a;

}
