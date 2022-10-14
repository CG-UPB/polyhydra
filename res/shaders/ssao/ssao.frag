#version 330 core

in vec2             v_uv;

// customization, tweak these values to your liking
uniform int         u_samples;
uniform float       u_radius;
uniform float       u_strength;
uniform float       u_bias;
uniform int         u_noise_size;
uniform float       u_distance_bias;

// variables for texture sampling
uniform vec4        u_sample_kernel[64];
uniform int         u_viewport_width;
uniform int         u_viewport_height;
uniform mat4        u_projection;
uniform mat4        u_inv_projection;
uniform mat4        u_view;
uniform float       u_far;

// world space position, normal and noise textures
uniform sampler2D   u_depth;
uniform sampler2D   u_normal;
uniform sampler2D   u_noise;

out float           o_occlusion;

vec4 get_position(vec2 uv)
{
    vec4 point = u_inv_projection * vec4(uv.xy * 2.0 - 1.0, texture(u_depth, uv).x * 2.0 - 1.0, 1.0);
    return point / point.w;
}

void main()
{
    vec3 position = get_position(v_uv).xyz;
    if (position.z < -u_far * 0.2)
    {
        o_occlusion = 1.0;
        return;
    }

    // sample textures to get the fragments view space position, normal and a random vector to rotate our kernel
    vec3 frag_pos = vec4(u_view * vec4(position, 1.0)).xyz;

    // since our noise texture is small, we need to tile it to the screen
    vec2 noise_scale = vec2(float(u_viewport_width), float(u_viewport_height)) / u_noise_size;
    vec3 random_vec = texture(u_noise, v_uv * noise_scale).xyz;
    vec3 normal = texture(u_normal, v_uv).xyz;
    vec3 frag_pos_biased = frag_pos + normal * 0.01;

    // create orthogonal basis to transform samples into tangent space of the fragment
    vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < u_samples; i++)
    {
        // transform kernel sample to tangent space
        vec3 sample_pos = TBN * u_sample_kernel[i].xyz;
        float sample_weight = u_sample_kernel[i].w;
        sample_pos = frag_pos_biased + sample_pos * u_radius;

        // transform sample to screen space, so we can look up the depth of our sample in the texture
        vec4 offset = u_projection * vec4(sample_pos, 1.0);
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        // no need to sample points outside our screen
        if (offset.x < 0.0 || offset.x > 1.0 || offset.y < 0.0 || offset.y > 1.0)
        {
            continue;
        }

        // now get the depth value of our sample
        vec3 sample_xyz = get_position(offset.xy).xyz;
        float sample_depth = vec4(u_view * vec4(sample_xyz, 1.0)).z;

        // check if the depth we sampled is within the sample radius, and smoothly interpolate
        float range_check = smoothstep(0.0, 1.0, u_radius / abs(frag_pos_biased.z - sample_depth));
        occlusion += (sample_depth >= sample_pos.z + u_bias ? sample_weight : 0.0) * range_check;
    }

    // finally, adjust the occlusion factor's strength by potentiation
    occlusion = 1.0 - (occlusion / u_samples);
    occlusion = pow(occlusion, u_strength);
    o_occlusion = occlusion;
}