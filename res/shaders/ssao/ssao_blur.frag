#version 330 core

in vec2 v_uv;

uniform sampler2D u_ssao_input;
uniform sampler2D u_position;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform int u_noise_size;

uniform float u_blur_threshold;

out float o_ao_factor;

void main()
{
    // there is no object here, so no need to blur
    vec3 position = texture(u_position, v_uv).xyz;
    float depth = position.z;
    if (depth == 0.0)
    {
        o_ao_factor = 1.0;
        return;
    }
    float projected_depth = vec4(u_projection * u_view * vec4(vec3(position), 1.0)).z;
    vec2 texel_size = 1.0 / vec2(textureSize(u_ssao_input, 0));
    float result = 0.0;
    float samples = 0.0;
    int size = u_noise_size / 2;
    for (int x = -size; x < size; x++)
    {
        for (int y = -size; y < size; y++)
        {
            vec2 offset_uv = v_uv + vec2(float(x), float(y)) * texel_size;
            float sample_depth = texture(u_position, offset_uv).z;
            if (abs(sample_depth - depth) < u_blur_threshold * 1.0 / ((projected_depth + 1.0) * 0.5))
            {
                result += texture(u_ssao_input, offset_uv).r;
                samples += 1.0;
            }
        }
    }
    o_ao_factor = result / samples;
}