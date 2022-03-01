#version 330 core

layout(location = 0) out float FragColor;

uniform vec4 u_object_color;

void main()
{
    if(u_object_color.a != 1.0)
    {
        discard;
    }
}