#version 330 core

layout (location = 0) out vec3 normal;
layout (location = 1) out vec3 position;

in vec3 v_pos;
in vec3 v_normal;
flat in int v_visible;

void main()
{
    if (v_visible == 0)
    {
        discard;
    }
    normal = normalize(v_normal);
    position = v_pos;
}