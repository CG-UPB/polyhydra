#version 330 core

layout(location = 0) out vec4 FragColor;

uniform vec4 u_object_color;
uniform sampler2D u_opaque_depth_texture;

void main()
{
    float max_depth = texelFetch(u_opaque_depth_texture, ivec2(gl_FragCoord.xy), 0).r;

    if(u_object_color.a == 1.0 || gl_FragCoord.z > max_depth)
    {
        discard;
    }

    FragColor = vec4(vec3(u_object_color.a * u_object_color.rgb), u_object_color.a);
}