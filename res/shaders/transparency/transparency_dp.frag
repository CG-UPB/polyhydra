#version 400 core

in vec2 v_uv;

layout (location = 0) out vec4 FragColor;

uniform vec4 u_object_color;
uniform sampler2DMS last_depth_texture;

void main()
{
    vec4 color = u_object_color;
    float frag_depth = gl_FragCoord.z;

    float last_depth = texelFetch(last_depth_texture, ivec2(gl_FragCoord.xy), 0).r;

    if(frag_depth >= last_depth || color.a == 1.0)
    {
        discard;
    }

    FragColor = color;

}