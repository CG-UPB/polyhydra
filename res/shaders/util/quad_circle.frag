#version 330 core

in vec2 tex_coord;

uniform float u_gamma;
uniform vec4 u_hover_color;

out vec4 fragmentColor;

void main()
{
    vec2 uv = tex_coord * 2.0 - 1.0;
    float distance = 1.0 - length(uv);

    // anti aliasing
    fragmentColor = vec4(smoothstep(0.0, 0.1, distance));
    fragmentColor *= u_hover_color;
    fragmentColor.rgb = pow(fragmentColor.rgb, vec3(1.0 / u_gamma));
}