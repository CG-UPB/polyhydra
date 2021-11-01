#version 330 core

in vec2 texCoord;

out vec4 FragColor;

void main()
{
    FragColor = vec4(texCoord.xy, 0.2, 1.0);
}