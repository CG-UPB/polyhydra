#version 400 core

layout (location = 0) out vec4 FragColor;

in vec2 v_uv;

uniform sampler2D accumTexture;
uniform sampler2D revealTexture;

const float EPSILON = 0.00001f;

bool is_approximately_equal(float a, float b)
{
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

float max_component(vec3 v)
{
    return max(max(v.x,v.y),v.z);
}

void main()
{
    vec2 coord = v_uv;

    vec4 accum = texelFetch(accumTexture, ivec2(gl_FragCoord.xy), 0);

    float revealage = texelFetch(revealTexture, ivec2(gl_FragCoord.xy), 0).r;

    if (is_approximately_equal(revealage, 1.0f))
    {
        discard;
    }



    // supress overflow
    if (isinf(max_component(abs(accum.rgb))))
    {
        accum.rgb = vec3(accum.a);
    }

    vec3 average_color = accum.rgb / max(accum.a, EPSILON);

    FragColor = vec4(average_color, 1.0f - revealage);
    //FragColor.rgb = pow(FragColor.rgb, vec3(1.0/2.2));

}