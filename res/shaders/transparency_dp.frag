#version 400 core

uniform vec4 u_objectColor;
uniform sampler2D opaqueTexture;
uniform sampler2D transparentZTexture;

in vec2 v_uv;

layout (location = 0) out vec4 FragColor;

void main()
{
    vec2 coord = v_uv;
    float frag_depth = gl_FragCoord.z;

    float odepth = texelFetch(opaqueTexture, coord, 0);
    float tdepth = texelFetch(transparentZTexture, coord, 0);

    if(frag_depth >= odepth || frag_depth <= tdepth)
    {
        discard;
    }
}