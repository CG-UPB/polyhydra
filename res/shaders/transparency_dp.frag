#version 400 core

uniform sampler2D old_min_texture;
uniform sampler2D max_texture;
uniform vec4 u_object_color;

in vec2 v_uv;

layout (location = 0) out float min_depth_texture;
layout (location = 1) out vec4 front_texture;
layout (location = 2) out vec4 back_texture;

void main()
{
    vec4 color = u_object_color;
    vec2 coord = v_uv;
    float frag_depth = gl_FragCoord.z;


    float max_depth = texelFetch(max_texture, coord, 0);
    float min_depth = texelFetch(old_min_texture, coord, 0);

    if(frag_depth >= old_max_depth || frag_depth <= old_min_depth)
    {
        discard;
    }



    front_texture = vec4(color.rgb * color.a, color.a);
    back_texture = u_object_color;

}