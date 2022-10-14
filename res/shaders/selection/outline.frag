#version 330 core

flat in int v_visible;
flat in int v_selected;

uniform vec4 u_outline_color;
uniform bool u_draw_outline;

out vec4 o_fragment_color;

void main()
{
    if (v_visible == 0 || v_selected == 0)
    {
        discard;
    }
    o_fragment_color = u_outline_color;
    if (!u_draw_outline)
    {
        o_fragment_color = vec4(0.5, 0.5, 0.5, 1.0);
    }
}