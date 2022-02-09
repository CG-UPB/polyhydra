#version 330 core

layout (location = 0) out vec4 FragColor;
//layout (location = 1) in vec4 peel_depth;
//layout (location = 1) out vec4 new_peel_depth;


uniform sampler2D u_min_depth;
uniform vec4 u_objectColor;

void main()
{
    vec2 coords = gl_FragCoord.xy;
    float frag_depth = gl_FragCoord.z;
    float min_depth = texture(u_min_depth, coords, 0).r;

//    if (u_objectColor.a == 1.0 || frag_depth <= min_depth || frag_depth >= peel_depth.r)
//    {
//        discard;
//    }
    //new_peel_depth.r = gl_FragCoord.z;

    vec4 color = u_objectColor;
    float a = color.a;
    FragColor = vec4(color.r * a, color.g * a, color.b * a, a);
    FragColor = color;

}