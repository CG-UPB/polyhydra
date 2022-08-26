#version 330 core

flat in int v_discard;
flat in int v_vertex_id;
flat in int v_visible;

uniform vec4 u_color;

out vec4 fragment_color;

void main()
{
    if (v_discard == 1 || v_visible == 0)
    {
        discard;
    }
    fragment_color = vec4(u_color.rgb, 1.0);
}