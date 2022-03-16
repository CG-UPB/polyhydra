#version 330 core

layout (location = 0) out vec3 normal;
layout (location = 1) out vec3 position;

in vec3 v_pos;
in vec3 v_normal;
in vec4 v_color;
flat in int v_visible;

uniform mat4 u_view;

void main()
{
    if (v_visible == 0 || v_color.a < 1.0 - 0.01)
    {
        discard;
    }
    normal = normalize(v_normal);
    position = vec3(inverse(u_view) * vec4(v_pos, 1.0));
}