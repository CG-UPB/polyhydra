#version 330 core

in vec2 v_uv;

uniform float u_gamma;
uniform sampler2D u_image;

out vec4 FragColor;

void main()
{
    vec4 color = texture(u_image, v_uv);
    FragColor = vec4(pow(color.rgb, vec3(1.0 / u_gamma)), color.a);
}