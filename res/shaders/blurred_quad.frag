#version 330 core

in vec2 tex_coord;

uniform vec4 u_hover_color;

out vec4 fragmentColor;

void main()
{
    vec2 uv = tex_coord * 2.0 - 1.0;
    float distance = 1.0 - length(uv);

    // anti aliasing
    fragmentColor = vec4(smoothstep(0.0, 0.3, distance));
    fragmentColor *= u_hover_color;
}