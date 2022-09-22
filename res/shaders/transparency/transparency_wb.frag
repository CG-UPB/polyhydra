#version 400 core
#define PI 3.14159265359

layout (location = 0) out vec4 accum;
layout (location = 1) out float reveal;
layout (location = 2) out vec3 modulate;

in vec3 v_pos;
in vec3 v_normal;
in vec4 v_color;
flat in int v_visible;

uniform vec3 u_light_pos;
uniform vec3 u_cam_pos;
uniform vec3 u_light_color;
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

// phong lighting model
uniform float u_spec_strength;
uniform float u_ambient_strength;
uniform float u_diffuse_strength;
uniform float u_spec_exponent;

// pbr lighting model
uniform bool u_use_pbr;
uniform float u_metallic;
uniform float u_roughness;
uniform float u_gamma;
uniform vec3 u_ground_color;
uniform vec3 u_background_color;


float near = 0.1f;
float far = 100.0f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

vec3 calculate_phong_lighting(vec3 color, vec3 n, vec3 l, float ao, float shadow)
{
    vec3 ambient = u_ambient_strength * u_light_color * ao;

    //diffuse
    float diff = max(0.0, dot(l, n));
    vec3 diffuse = u_diffuse_strength * diff * u_light_color;

    //specular
    vec3 v = normalize(u_cam_pos - v_pos);
    vec3 r = reflect(-l, n);
    float spec = pow(max(0.0, dot(v, r)), u_spec_exponent);
    vec3 specular = u_spec_strength * spec * u_light_color;

    float norm = u_ambient_strength + u_diffuse_strength + u_spec_strength;
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) / norm * color;
    return result;
}

vec3 fresnel_schlick(float cos_theta, vec3 f0)
{
    return f0 + (1.0 - f0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

vec3 fresnel_schlick_roughness(float cos_theta, vec3 f0, float roughness)
{
    return f0 + (max(vec3(1.0 - roughness), f0) - f0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

float distribution_ggx(vec3 n, vec3 h, float roughness)
{
    float a         = roughness * roughness;
    float a2        = a * a;
    float n_dot_h   = max(dot(n, h), 0.0);
    float n_dot_h2  = n_dot_h * n_dot_h;

    float num       = a2;
    float denom     = (n_dot_h2 * (a2 - 1.0) + 1.0);
    denom           = PI * denom * denom;

    return num / denom;
}

float geometry_schlick_ggx(float n_dot_v, float roughness)
{
    float r         = (roughness + 1.0);
    float k         = (r * r) / 8.0;

    float num       = n_dot_v;
    float denom     = n_dot_v * (1.0 - k) + k;

    return num / denom;
}

float geometry_schlick_ggx_env(float n_dot_v, float roughness)
{
    float r         = roughness;
    float k         = (r * r) / 2.0;

    float num       = n_dot_v;
    float denom     = n_dot_v * (1.0 - k) + k;

    return num / denom;
}

float geometry_smith(vec3 n, vec3 v, vec3 l, float roughness)
{
    float n_dot_v = max(dot(n, v), 0.0);
    float n_dot_l = max(dot(n, l), 0.0);
    float ggx2 = geometry_schlick_ggx(n_dot_v, roughness);
    float ggx1 = geometry_schlick_ggx(n_dot_l, roughness);

    return ggx1 * ggx2;
}

float geometry_smith_env(vec3 n, vec3 v, vec3 l, float roughness)
{
    float n_dot_v = max(dot(n, v), 0.0);
    float n_dot_l = max(dot(n, l), 0.0);
    float ggx2 = geometry_schlick_ggx_env(n_dot_v, roughness);
    float ggx1 = geometry_schlick_ggx_env(n_dot_l, roughness);

    return ggx1 * ggx2;
}

vec3 get_environment_color(vec3 direction)
{
    return mix(u_ground_color, u_background_color, direction.y * 0.5 + 0.5);
}

float radical_inverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), radical_inverse_VdC(i));
}

vec3 importance_sample_ggx(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness * roughness;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

vec3 get_pre_filtered_color(vec3 localPos, float roughness)
{
    vec3 N = normalize(localPos);
    vec3 R = N;
    vec3 V = R;

    const uint SAMPLE_COUNT = 8u;
    float totalWeight = 0.0;
    vec3 prefilteredColor = vec3(0.0);
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = hammersley(i, SAMPLE_COUNT);
        vec3 H  = importance_sample_ggx(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0)
        {
            prefilteredColor += get_environment_color(L) * NdotL;
            totalWeight      += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;

    return prefilteredColor;
}

vec2 integrate_brdf(float n_dot_v, float roughness)
{
    vec3 V;
    V.x = sqrt(1.0 - n_dot_v * n_dot_v);
    V.y = 0.0;
    V.z = n_dot_v;

    float A = 0.0;
    float B = 0.0;

    vec3 N = vec3(0.0, 0.0, 1.0);

    const uint SAMPLE_COUNT = 8u;
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = hammersley(i, SAMPLE_COUNT);
        vec3 H  = importance_sample_ggx(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if(NdotL > 0.0)
        {
            float G = geometry_smith_env(N, V, L, roughness);
            float G_Vis = (G * VdotH) / (NdotH * n_dot_v + 0.0001);
            float Fc = pow(1.0 - VdotH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    return vec2(A, B);
}

vec3 calculate_pbr_lighting(vec3 albedo, vec3 n, vec3 l, vec3 v, float ao, float shadow)
{
    vec3 f0 = vec3(0.04);
    f0 = mix(f0, albedo, u_metallic);

    // reflectance
    vec3 lo = vec3(0.0);

    // radiance
    vec3 h = normalize(v + l);

    // we have a directional light, so no attenuation
    float attenuation = 1.0;
    vec3 radiance = u_light_color * 5.0 * attenuation;

    // cook-torrance brdf
    float ndf = distribution_ggx(n, h, u_roughness);
    float g = geometry_smith(n, v, l, u_roughness);
    vec3 f = fresnel_schlick(max(dot(h, v), 0.0), f0);

    vec3 ks = f;
    vec3 kd = vec3(1.0) - ks;
    kd *= 1.0 - u_metallic;

    vec3 numerator = ndf * g * f;
    float denominator = 4.0 * max(dot(n, v), 0.0) * max(dot(n, l), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    // outgoing radiance
    float n_dot_l = max(dot(n, l), 0.0);
    lo += (kd * albedo / PI + specular) * radiance * n_dot_l;

    vec3 ks2 = fresnel_schlick_roughness(max(dot(n, v), 0.0), f0, u_roughness);
    vec3 kd2 = 1.0 - ks2;
    kd2 *= 1.0 - u_metallic;

    vec3 irradiance = mix(u_ground_color, u_background_color, n.y * 0.5 + 0.5);
    vec3 diffuse = irradiance * albedo;

    vec3 specular_color = get_environment_color(reflect(-v, n));
    vec3 indirect_specular = specular_color * ks2 * 0.5;

    vec3 ambient = (kd2 * diffuse + indirect_specular) * ao;

    vec3 result = ambient + lo * (1.0 - shadow);

    result = result / (result + vec3(1.0));
    return result;
}

void main()
{
    vec3 used_color = vec3(0.0);


    vec3 n = normalize(v_normal);
    vec3 l = normalize(u_light_pos);

    if(v_color.a >= 1.0 - 0.01|| v_visible == 0)
    {
        discard;
    }
    vec3 light_color = u_light_color;

    vec2 uv = gl_FragCoord.xy / vec2(u_viewport_width, u_viewport_height);

    vec3 result;
    if (u_use_pbr)
    {
        vec3 v = normalize(u_cam_pos - v_pos);
        vec3 light = normalize(u_light_pos - v_pos);
        result = calculate_pbr_lighting(used_color, n, light, v, 1.0f, 0.0f);
    }
    else
    {
        result = calculate_phong_lighting(used_color, n, l, 1.0f, 0.0f);
    }
    used_color.rgb = result;

    float depth = gl_FragCoord.z;
    //float depth = LinearizeDepth(gl_FragCoord.z) / far;

    //color.a = pow(color.a, 2.0);

    // choose weight function
    float weight = clamp(pow(min(1.0, v_color.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - depth * 0.5, 3.0), 1e-2, 3e3);
    //float weight = pow(color.a, u_pow) * clamp(u_range / (1e-5 + pow(depth * 0.8, u_ordering_strenth)), u_t_min, u_t_max);
    //float weight = pow(color.a + 0.01, 4.0) + max(1e-2, min(3.0 * 1e3, 100.0 / (1e-5 + pow(abs(depth) / 10.0, 3.0) + pow(abs(depth) / 200.0, 6.0))));

    //float weight = 1.0 / pow(1.0 + LinearizeDepth(depth), u_pow);

    accum = vec4(used_color.rgb * v_color.a, v_color.a) * weight;
    reveal = v_color.a;

}