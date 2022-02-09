#version 330 core

layout (location = 0) out vec4 accum;
layout (location = 1) out float reveal;

uniform vec4 u_object_color;

void main()
{
    if(u_object_color.a == 1.0)
    {
        discard;
    }
    vec4 color = u_object_color;

    // choose weight function
    float weight = clamp(pow(min(1.0, color.a * 10.0) + 0.01, 3.0) * 1e08 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);

    accum = vec4(color.rgb * color.a, color.a) * weight;

    reveal = color.a;

}