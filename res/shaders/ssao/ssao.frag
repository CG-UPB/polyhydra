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
uniform vec3        u_sample_kernel[64];
uniform int         u_viewport_width;
uniform int         u_viewport_height;
uniform mat4        u_projection;
uniform mat4        u_view;
uniform float       u_far;

// world space position, normal and noise textures
uniform sampler2D   u_position;
uniform sampler2D   u_normal;
uniform sampler2D   u_noise;

out float           o_occlusion;

// sample position at uv coordinates of a given mip level
vec3 get_position(vec2 uv, float mip_level)
{
    return textureLod(u_position, uv, mip_level).xyz;
}

void main()
{
    vec3 position = get_position(v_uv, 0.0);
    // this fragment is not an object, so no need to occlude it to save performance
    if (position.z == -u_far)
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

    // create orthogonal basis to transform samples into tangent space of the fragment
    vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < u_samples; i++)
    {
        // transform kernel sample to tangent space
        vec3 sample_pos = TBN * u_sample_kernel[i];
        sample_pos = (frag_pos + normal * 0.01) + sample_pos * u_radius;

        // transform sample to screen space, so we can look up the depth of our sample in the texture
        vec4 offset = u_projection * vec4(sample_pos, 1.0);
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        // no need to sample points outside our screen
        if (offset.x < 0.0 || offset.x > 1.0 || offset.y < 0.0 || offset.y > 1.0)
        {
            continue;
        }

        // cache optimization:
        // sample different mip level based on the screen space distance of the two fragments.
        // this prevents cache misses and also improves sample quality, since the further the sample is from the
        // fragment, the less we care about the exact sample depth. A coarser approximation is sufficient here.
        float sample_dist = smoothstep(0.0, 1.0, length(offset.xy - v_uv) + 0.2);

        // the exponent changes the aggressivenes of the mip-mapping
        // the higher the exponent, the earlier we sample from lower mips, the less cache misses we have
        float level = floor(pow(1.0 + sample_dist, u_distance_bias)) - 1.0;

        // now get the depth value of our sample
        vec3 sample_xyz = get_position(offset.xy, level);
        float sample_depth = vec4(u_view * vec4(sample_xyz, 1.0)).z;

        // check if the depth we sampled is within the sample radius, and smoothly interpolate
        float range_check = smoothstep(0.0, 1.0, u_radius / abs(frag_pos.z - sample_depth));
        occlusion += (sample_depth >= sample_pos.z + u_bias ? 1.0 : 0.0) * range_check;
    }

    // finally, adjust the occlusion factor's strength by potentiation
    occlusion = 1.0 - (occlusion / u_samples);
    occlusion = pow(occlusion, u_strength);
    o_occlusion = occlusion;
}