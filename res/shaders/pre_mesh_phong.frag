#version 330 core

layout (location = 0) out vec4 color0;
layout (location = 1) out vec4 color1;

flat in int v_visible;

void main()
{
    if (v_visible == 0)
    {
        discard;
    }

    color0 = vec4(1.0, 0.5, 0.0, 1.0);
    color1 = vec4(0.0, 0.5, 1.0, 1.0);
}