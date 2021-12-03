#version 330 core

flat in int v_discard;

out vec4 fragment_color;

void main()
{
    if (v_discard == 1)
    {
        discard;
    }
    fragment_color = vec4(0.0, 1.0, 0.0, 1.0);
}