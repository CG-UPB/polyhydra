#version 330 core

const float far = 1000.0;

in vec2 v_uv;

uniform int u_viewport_width;
uniform int u_viewport_height;

uniform sampler2D u_ssao_input;
uniform sampler2D u_position;
uniform int u_noise_size;

out float o_ao_factor;

void main()
{
    vec3 position = textureLod(u_position, v_uv, 0.0).xyz;
    float depth = position.z;
    // there is no object here, so no need to blur
    if (depth == -far)
    {
        o_ao_factor = 1.0;
        return;
    }
    float center_factor = texture(u_ssao_input, v_uv).r;
    vec2 texel_size = 1.0 / vec2(float(u_viewport_width), float(u_viewport_height));
    float result = 0.0;
    float samples = 0.0;
    float size = float(u_noise_size) * 0.5;
    for (float x = -size; x < size; x += 1.0)
    {
        for (float y = -size; y < size; y += 1.0)
        {
            vec2 offset_uv = v_uv + vec2(x, y) * texel_size;
            float sample_factor = texture(u_ssao_input, offset_uv).r;
            float diff = abs(sample_factor - center_factor);
            float weight = 1.0 / pow(1.0 + diff, 2.0);
            result += sample_factor * weight;
            samples += weight;
        }
    }
    o_ao_factor = result / samples;
}