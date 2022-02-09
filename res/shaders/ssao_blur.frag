#version 330 core

in vec2 v_uv;

uniform sampler2D u_ssao_input;

out float FragColor;

void main()
{
    vec2 texel_size = 1.0 / vec2(textureSize(u_ssao_input, 0));
    float result = 0.0;
    for (int x = -2; x < 2; x++)
    {
        for (int y = -2; y < 2; y++)
        {
            vec2 offset = vec2(float(x), float(y)) * texel_size;
            result += texture(u_ssao_input, v_uv + offset).r;
        }
    }
    FragColor = result / (4.0 * 4.0);
}