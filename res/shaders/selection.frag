#version 330 core

flat in int v_face_id;

out vec4 FragColor;

void main()
{
    // insert identifying color in here
    int r = (v_face_id & 0x000000FF) >>  0;
    int g = (v_face_id & 0x0000FF00) >>  8;
    int b = (v_face_id & 0x00FF0000) >> 16;
    int a = (v_face_id & 0xFF000000) >> 24;

    FragColor = vec4(float(r)/255.0, float(g)/255.0, float(b)/255.0, 1.0);
}