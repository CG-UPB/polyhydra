#version 400 core

in vec2 v_uv;

layout (location = 0) out vec4 FragColor;
uniform int u_max_passes;
uniform int u_current_pass;

uniform sampler2D new_layer_texture;

void main()
{

    ivec2 coords = ivec2(gl_FragCoord.xy);
    vec4 color = texelFetch(new_layer_texture, coords, 0);

    // Blend over filled Framebuffer
    if(u_current_pass == 0)
    {
        FragColor = color;
    }
    else
    {
        FragColor = vec4(color.a * color.rgb, color.a);
    }

    if(u_current_pass == u_max_passes)
    {
        FragColor.rgb = FragColor.rgb / FragColor.a;
    }
}