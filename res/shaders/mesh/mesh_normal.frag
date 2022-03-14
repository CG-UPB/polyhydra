#version 330 core

out vec4 FragColor;

flat in int v_Visible;
in vec3 v_Normal;

void main()
{
    if (v_Visible == 0)
    {
        discard;
    }
    FragColor = vec4(normalize(v_Normal), 1.0);
}