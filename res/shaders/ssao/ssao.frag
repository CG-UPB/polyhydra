#version 330 core

const float near = 0.1;
const float far  = 1000.0;

in vec2 v_uv;

uniform int u_samples;
uniform float u_radius;
uniform int u_strength;
uniform float u_bias;
uniform float u_screen_radius;

uniform vec3 u_sample_kernel[64];

uniform int u_viewport_width;
uniform int u_viewport_height;
uniform mat4 u_projection;
uniform mat4 u_view;

uniform sampler2D u_position;
uniform sampler2D u_normal;
uniform sampler2D u_noise;

out float o_occlusion;

void main()
{
    vec2 noise_scale = vec2(float(u_viewport_width) / 4.0, float(u_viewport_height) / 4.0);

    vec3 frag_pos = vec4(u_view * vec4(texture(u_position, v_uv).xyz, 1.0)).xyz;
    vec3 random_vec = texture(u_noise, v_uv * noise_scale).xyz;
    vec3 normal = texture(u_normal, v_uv).xyz;

    vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    float samples = 0.0;
    for (int i = 0; i < u_samples; i++)
    {
        vec3 sample_pos = TBN * u_sample_kernel[i];
        sample_pos = frag_pos + sample_pos * u_radius;
        vec4 offset = vec4(sample_pos, 1.0);
        offset = u_projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        // uncomment this to save performance
        // apparently, many GPUs struggle when we are sampling a texture at points that are too far away from each other
         vec2 sample_vec = offset.xy - v_uv;
         float screen_sample_dist = dot(sample_vec, sample_vec);
         if (screen_sample_dist > u_screen_radius)
         {
             continue;
         }

        // no need to sample points outside of our screen
        if (offset.x < 0.0 || offset.x > 1.0 || offset.y < 0.0 || offset.y > 1.0)
        {
            continue;
        }

        // we have hit nothing, so we can continue
        vec3 sample_xyz = texture(u_position, offset.xy).xyz;
        if (sample_xyz.z == 0.0)
        {
            continue;
        }

        float sample_depth = vec4(u_view * vec4(sample_xyz, 1.0)).z;
        float range_check = smoothstep(0.0, u_radius, u_radius / abs(frag_pos.z - sample_depth)) / u_radius;
        occlusion += (sample_depth >= sample_pos.z + u_bias ? 1.0 : 0.0) * range_check;
        samples += 1.0;
    }

    occlusion = 1.0 - (occlusion / samples);
    occlusion = pow(occlusion, u_strength);
    o_occlusion = occlusion;
}