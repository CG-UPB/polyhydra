#version 330 core

uniform int u_selection_offset;

out vec4 FragColor;

void main()
{
    int id = u_selection_offset + gl_PrimitiveID;
    // insert identifying color in here
    int r = (id & 0x000000FF) >>  0;
    int g = (id & 0x0000FF00) >>  8;
    int b = (id & 0x00FF0000) >> 16;
    int a = (id & 0xFF000000) >> 24;

    FragColor = vec4(float(r)/255.0, float(g)/255.0, float(b)/255.0, 1.0);
}