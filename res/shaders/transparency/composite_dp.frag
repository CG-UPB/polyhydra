#version 400 core

in vec2 v_uv;

layout (location = 0) out vec4 FragColor;

uniform sampler2D front_texture;
uniform sampler2D opaque_texture;

void main()
{
    ivec2 coords = ivec2(gl_FragCoord.xy);

    vec4 front = texelFetch(front_texture, coords, 0);
    vec4 opaque = texelFetch(opaque_texture, coords, 0);

    FragColor = vec4(front.a * front.rgb + (1 - front.a) * opaque.rgb, opaque.a);

}