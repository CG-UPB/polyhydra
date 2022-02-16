#version 400 core

layout (location = 0) out vec4 accum;
layout (location = 1) out vec4 reveal;
layout (location = 2) out vec3 modulate;

flat in int v_visible;

uniform vec4 u_object_color;
uniform int u_viewport_width;
uniform int u_viewport_height;

float near = 0.1f;
float far = 100.0f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    vec4 color = u_object_color;

    if(color.a == 1.0 || v_visible == 0)
    {
        discard;
    }


    float depth = gl_FragCoord.z;
    //float depth = LinearizeDepth(gl_FragCoord.z) / far;


    // choose weight function
    //float weight = clamp(pow(min(1.0, color.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - depth * 0.5, 3.0), 1e-2, 3e3);
    //float weight = max(min(1.0, max(max(color.r, color.g), color.b) * color.a)), color.a) * clamp(0.03 / (1e-5 + pow(depth / 200, 4.0), 1e-2, 3e3);
    float weight = color.a  * clamp(0.3 / (1e-5 + pow(depth /200, 4.0)), 1e-2, 3e3);


    accum = vec4(color.rgb * color.a, color.a) * weight;
    reveal = vec4(color.a, 0.0, 0.0, 1.0);

}