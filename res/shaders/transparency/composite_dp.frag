#version 400 core

in vec2 v_uv;

layout (location = 0) out vec4 FragColor;
uniform int u_max_passes;
uniform int u_current_pass;

uniform sampler2D new_layer_texture;

void main()
{

    ivec2 coords = ivec2(gl_FragCoord.xy);
    vec4 color = texelFetch(new_layer_texture, coords, 0);

    FragColor = vec4(color.r * color.a, color.g * color.a, color.b * color.a, color.a);
}