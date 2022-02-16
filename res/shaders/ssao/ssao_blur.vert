#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uv;

out vec2 v_uv;

void main()
{
    // flip uv's, since they are inverted
    v_uv = vec2(a_uv.x, 1.0 - a_uv.y);
    gl_Position = vec4(a_pos, 1.0);
}