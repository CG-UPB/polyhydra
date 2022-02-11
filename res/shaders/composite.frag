#version 400 core

layout (location = 0) out vec4 FragColor;

in vec2 v_uv;

uniform sampler2D accumTexture;
uniform sampler2D revealTexture;

const float EPSILON = 0.00001f;

float max_component(vec3 v)
{
    return max(max(v.x,v.y),v.z);
}

void main()
{
    vec2 coord = v_uv;
    float revealage = texelFetch(revealTexture, ivec2(gl_FragCoord.xy), 0).r;
    if (revealage == 1.0)
    {
        discard;
    }

    vec4 accum = texelFetch(accumTexture, ivec2(gl_FragCoord.xy), 0);

    // supress overflow
    if (isinf(max_component(abs(accum.rgb))))
    {
        accum.rgb = vec3(accum.a);
    }

    vec3 average_color = accum.rgb / max(accum.a, EPSILON);

    FragColor = vec4(average_color, 1.0-revealage);
    //FragColor = vec4(1.0, 0.1, 0.1, 1.0);
}