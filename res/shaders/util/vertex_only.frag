#version 330 core

flat in int v_discard;
flat in int v_vertex_id;
flat in int v_visible;

uniform float u_gamma;
uniform vec4 u_color;

out vec4 fragment_color;

void main()
{
    if (v_discard == 1 || v_visible == 0)
    {
        discard;
    }
    fragment_color = vec4(pow(u_color.rgb, vec3(1.0 / u_gamma)), 1.0);
}