#version 330 core

in vec2 v_uv;

uniform int u_viewport_width;
uniform int u_viewport_height;
uniform mat4 u_inv_projection;
uniform float u_far;

uniform float u_sharpness;

uniform sampler2D u_ssao_input;
uniform sampler2D u_depth;
uniform int u_noise_size;

out float o_ao_factor;

vec4 get_position(vec2 uv)
{
    vec4 point = u_inv_projection * vec4(uv.xy * 2.0 - 1.0, texture(u_depth, uv).x * 2.0 - 1.0, 1.0);
    return point / point.w;
}

float blur(vec2 uv, float radius, float center_depth, inout float total_weight)
{
    float factor = texture(u_ssao_input, uv).x;
    float depth = texture(u_depth, uv).x;
    float blur_sigma = float(u_noise_size) * 0.5;
    float blur_falloff = 1.0 / (2.0 * blur_sigma * blur_sigma);
    float depth_diff = (depth - center_depth) * u_sharpness * 100.0;
    float weight = exp2(-radius * radius * blur_falloff - depth_diff * depth_diff);
    total_weight += weight;
    return factor * weight;
}

void main()
{
    vec3 position = get_position(v_uv).xyz;
    float depth = position.z;
    // there is no object here, so no need to blur
    if (depth == -u_far)
    {
        o_ao_factor = 1.0;
        return;
    }
    float center_factor = texture(u_ssao_input, v_uv).x;
    float center_depth = texture(u_depth, v_uv).x;
    float total_factor = center_factor;
    float total_weight = 1.0;
    vec2 texel_size = 1.0 / vec2(float(u_viewport_width), float(u_viewport_height));
    float size = float(u_noise_size) * 0.5;
    for (float x = -size; x < size; x += 1.0)
    {
        for (float y = -size; y < size; y += 1.0)
        {
            vec2 offset = vec2(x, y);
            vec2 offset_uv = v_uv + offset * texel_size;
            total_factor += blur(offset_uv, length(offset), center_depth, total_weight);
        }
    }
    o_ao_factor = total_factor / total_weight;
}