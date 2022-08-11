#version 330 core

in vec2 v_uv;

uniform sampler2DArray u_depth_texture;
uniform int u_cascade_level;

out vec4 FragColor;

void main()
{
    float depth = texture(u_depth_texture, vec3(v_uv, float(u_cascade_level))).r;
    FragColor = vec4(depth, depth, depth, 1.0);
}