#version 400 core

uniform sampler2D front_texture;
uniform sampler2D back_texture;
uniform sampler2D opaque_texture;

in vec2 v_uv;

layout (location = 0) out vec4 FragColor;


void main()
{
    vec2 coord = v_uv;

    vec4 front = texelFetch(front_texture, ivec2(gl_FragCoord.xy), 0);
    vec4 back = texelFetch(back_texture, ivec2(gl_FragCoord.xy), 0);
    vec4 opaque = texelFetch(opaque_texture, ivec2(gl_FragCoord.xy), 0);

    vec4 accum = vec4(front.a * front.rgb + (1 - front.a) * back.rgb, back.a);
    FragColor = vec4(accum.a * opaque.rgb + accum.rgb, 1.0);

}