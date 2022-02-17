#version 400 core

layout (location = 0) out vec4 accum;
layout (location = 1) out float reveal;
layout (location = 2) out vec3 modulate;

in vec3 v_pos;
in vec3 v_normal;
in vec4 v_color;
flat in int v_visible;

uniform vec3 u_lightPos;
uniform vec3 u_camPos;
uniform vec3 u_lightColor;
uniform vec4 u_object_color;
uniform int u_viewport_width;
uniform int u_viewport_height;

uniform float u_alpha_pow;
uniform float u_pow;
uniform float u_range;
uniform float u_depth_range;
uniform float u_ordering_strenth;
uniform float u_t_min;
uniform float u_t_max;

uniform sampler2D u_depth;
uniform sampler2D u_ssao_texture;

float near = 0.1f;
float far = 100.0f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    float previous_depth = texelFetch(u_depth, ivec2(gl_FragCoord.xy), 0).r;
    if (gl_FragCoord.z > previous_depth)
    {
        discard;
    }

    vec4 color = u_object_color;

    if(color.a == 1.0 || v_visible == 0)
    {
        discard;
    }

    vec2 uv = gl_FragCoord.xy / vec2(u_viewport_width, u_viewport_height);

    //ambient
    float ambientStrength = 1.0;
    float ao_factor = texture(u_ssao_texture, uv).r;
    vec3 ambient = ambientStrength * u_lightColor * (ao_factor * 0.1 + 0.9);

    // Phong Shading

    vec3 used_color = mix(u_object_color.rgb, vec3(v_color.x,v_color.y,v_color.z), v_color.w);

    //diffuse
    float diffuseStrength = 1.0;
    vec3 n = -normalize(v_normal);
    //vec3 l = normalize(u_lightPos - v_pos);
    // constant light direction looks way better than a single point of light
    vec3 l = normalize(vec3(0.0, -1.0, -1.0));
    float diff = max(0.0, dot(l, n));
    vec3 diffuse = diffuseStrength * diff * u_lightColor;

    //specular
    float specularStrength = 0.2;
    vec3 v = normalize(u_camPos - v_pos);
    vec3 r = reflect(-l, n);
    float spec = pow(max(0.0, dot(v, r)), 8);
    vec3 specular = specularStrength * spec * u_lightColor;

    float norm = ambientStrength + diffuseStrength + specularStrength;
    color.rgb = (ambient + diffuse + specular) / norm * used_color;


    float depth = gl_FragCoord.z;
    //float depth = LinearizeDepth(gl_FragCoord.z) / far;

    color.a = pow(color.a, 2.0);

    // choose weight function
    //float weight = clamp(pow(min(1.0, color.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - depth * 0.5, 3.0), 1e-2, 3e3);
    //float weight = max(min(1.0, max(max(color.r, color.g), color.b) * color.a)), color.a) * clamp(0.03 / (1e-5 + pow(depth / 200, 4.0), 1e-2, 3e3);
    float weight = pow(color.a, u_pow) * clamp(u_range / (1e-5 + pow(depth * 0.8, u_ordering_strenth)), u_t_min, u_t_max);
    //float weight = pow(color.a + 0.01, 4.0) + max(1e-2, min(3.0 * 1e3, 100.0 / (1e-5 + pow(abs(depth) / 10.0, 3.0) + pow(abs(depth) / 200.0, 6.0))));

    accum = vec4(color.rgb * color.a, color.a) * weight;
    reveal = color.a;

}