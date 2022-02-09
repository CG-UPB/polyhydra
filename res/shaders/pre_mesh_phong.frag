#version 330 core

layout (location = 0) out vec4 normal;
layout (location = 1) out vec4 position;

in vec3 v_pos;
in vec3 v_normal;
flat in int v_visible;

void main()
{
    if (v_visible == 0)
    {
        discard;
    }

    normal = vec4(v_normal, 1.0);
    position = vec4(v_pos, 1.0);
}