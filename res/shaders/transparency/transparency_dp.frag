#version 400 core

in vec2 v_uv;

layout (location = 0) out vec4 front_texture;

uniform sampler2D min_texture;
uniform sampler2D max_texture;
uniform vec4 u_object_color;

void main()
{
    vec4 color = u_object_color;
    ivec2 coords = ivec2(gl_FragCoord.xy);
    float frag_depth = gl_FragCoord.z;

    float max_depth = texelFetch(max_texture, coords, 0).r;
    float min_depth = texelFetch(min_texture, coords, 0).r;

    if(frag_depth >= max_depth || frag_depth <= min_depth || color.a == 1.0f)
    {
        discard;
    }

    front_texture = vec4(color.rgb * color.a, color.a);

}