#version 330 core

in vec2 tex_coord;

uniform vec4 u_highlight_color;

out vec4 fragmentColor;

void main()
{
    fragmentColor = vec4(1.0);
    vec2 uv = tex_coord * 2.0 - 1.0;
    float distance = 1.0 - length(uv);

    // anti alialising
    vec3 color = vec3(smoothstep(0.0, 0.05, distance));
    color *= vec3(1.0 - step(1.0, distance));

    if (length(color) == 0.0)
    {
        discard;
    }

    fragmentColor.rgb = color;
    fragmentColor *= u_highlight_color;
}