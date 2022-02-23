#version 400 core

const float EPSILON = 0.00001f;

// sum(rgb * a, a)
uniform sampler2D accumTexture;
// prod(1 -a)
uniform sampler2D revealTexture;

in vec2 v_uv;

layout (location = 0) out vec4 FragColor;

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
    ivec2 coords = ivec2(gl_FragCoord.xy);

    float revealage = texelFetch(revealTexture, coords, 0).r;

    if (is_approximately_equal(revealage, 1.0f))
    {
        discard;
    }

    vec4 accum = texelFetch(accumTexture, coords, 0);

    // supress overflow
    if (isinf(max_component(abs(accum.rgb))))
    {
        accum.rgb = vec3(accum.a);
    }

    vec3 average_color = accum.rgb / max(accum.a, EPSILON);

    // dst' = (accum.rgb / accum.a) * (1 - revealage) + dst
    FragColor = vec4(average_color, 1.0 - revealage);

}