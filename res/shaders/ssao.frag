#version 330 core

const float near = 0.1;
const float far  = 1000.0;

in vec2 v_uv;

uniform int u_samples;
uniform float u_radius;
uniform int u_strength;
uniform float u_bias;

uniform vec3 u_sample_kernel[64];

uniform int u_viewport_width;
uniform int u_viewport_height;
uniform mat4 u_projection;
uniform mat4 u_view;

uniform sampler2D u_depth;
uniform sampler2D u_position;
uniform sampler2D u_normal;
uniform sampler2D u_noise;

out vec4 FragColor;

float linearize_depth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

vec3 get_view_space_position(float linear_depth)
{
    return vec3(v_uv.x * 2.0 - 1.0, v_uv.y * 2.0 - 1.0, linear_depth);
}

void main()
{
    vec2 noise_scale = vec2(float(u_viewport_width) / 4.0, float(u_viewport_height) / 4.0);

    vec3 frag_pos = vec4(u_view * texture(u_position, v_uv)).xyz;
    vec3 random_vec = texture(u_noise, v_uv * noise_scale).xyz;
    vec3 normal = texture(u_normal, v_uv).xyz;

    vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < u_samples; i++)
    {
        vec3 sample_pos = TBN * u_sample_kernel[i];
        sample_pos = frag_pos + sample_pos * u_radius;
        vec4 offset = vec4(sample_pos, 1.0);
        offset = u_projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        float sample_depth = texture(u_position, offset.xy).z;
        float range_check = smoothstep(0.0, 1.0, u_radius / abs(frag_pos.z - sample_depth));
        occlusion += (sample_depth >= sample_pos.z + u_bias ? 1.0 : 0.0) * range_check;
    }

    occlusion = 1.0 - (occlusion / u_samples);
    occlusion = pow(occlusion, u_strength);
    FragColor = vec4(vec3(occlusion), 1.0);
}