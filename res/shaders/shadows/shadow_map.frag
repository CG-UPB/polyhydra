#version 400 core

layout (location = 0) out vec4 FragColor;

flat in int v_visible;

void main()
{
    if (v_visible == 0 )
    {
        discard;
    }
}