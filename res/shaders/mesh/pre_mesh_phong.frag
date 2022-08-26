#version 330 core

layout (location = 0) out vec3 normal;

in vec3 v_normal;
flat in int v_visible;

void main()
{
    if (v_visible == 0)
    {
        discard;
    }
    normal = v_normal;
}