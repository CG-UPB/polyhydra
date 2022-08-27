#version 330 core

in vec2 v_uv;

uniform float u_saturation;
uniform float u_contrast;

uniform float u_gamma;
uniform sampler2D u_image;

out vec4 FragColor;

vec3 saturate(vec3 color, float saturation) {
    vec3 luminance = vec3(0.3086, 0.6094, 0.0820);
    float oneMinusSat = 1.0 - saturation;

    vec3 red = vec3(luminance.x * oneMinusSat);
    red.r += saturation;

    vec3 green = vec3(luminance.y * oneMinusSat);
    green.g += saturation;

    vec3 blue = vec3(luminance.z * oneMinusSat);
    blue.b += saturation;

    return mat3(red, green, blue) * color;
}

void main()
{
    vec4 color = texture(u_image, v_uv);
    color.rgb = ((color.rgb - 0.5f) * max(u_contrast, 0.0)) + 0.5f;
    color.rgb = saturate(color.rgb, u_saturation);
    FragColor = vec4(pow(color.rgb, vec3(1.0 / u_gamma)), color.a);
}