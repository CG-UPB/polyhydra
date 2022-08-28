#version 330 core

in vec3 v_normal;
flat in int v_visible;

layout (location = 0) out vec3 normal;

void main()
{
    if (v_visible == 0)
    {
        discard;
    }
    normal = v_normal;
}