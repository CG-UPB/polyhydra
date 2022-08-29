#version 330 core

#define PI 3.14159265359

const int MAX_CASCADE_LEVEL = 8;

in vec3 v_pos;
in vec3 v_normal;
in vec2 v_uv;
in vec4 v_pos_ls[MAX_CASCADE_LEVEL];
in float v_clipspace_z;

uniform bool u_visible;
uniform bool u_solid;
uniform vec3 u_solid_color;
uniform bool u_grid;
uniform vec3 u_grid_color;
uniform float u_height;
uniform int u_tile_count;
uniform bool u_shadow_only;

uniform vec3 u_light_pos;
uniform vec3 u_cam_pos;
uniform vec3 u_light_color;

uniform bool u_draw_shadows;
uniform bool u_draw_ao;

uniform float u_spec_strength;
uniform float u_ambient_strength;
uniform float u_diffuse_strength;
uniform float u_spec_exponent;

uniform bool u_use_pbr;
uniform float u_metallic;
uniform float u_roughness;
uniform float u_gamma;
uniform vec3 u_background_color;

uniform int u_cascade_level;
uniform float u_cascade_ends[MAX_CASCADE_LEVEL];
uniform mat4 u_view;

uniform int u_viewport_width;
uniform int u_viewport_height;

uniform float u_bias_min;
uniform float u_bias_max;
uniform float u_bias_modifier;

uniform sampler2D u_depth_texture;
uniform sampler2D u_ssao_texture;
uniform sampler2D u_transparent_shadow_texture;
uniform sampler2D u_color_filter_texture;
uniform sampler2DArrayShadow u_shadow_texture;

out vec4 FragColor;

vec2 poisson_disk[16] = vec2[](
    vec2( -0.94201624, -0.39906216 ),
    vec2( 0.94558609, -0.76890725 ),
    vec2( -0.094184101, -0.92938870 ),
    vec2( 0.34495938, 0.29387760 ),
    vec2( -0.91588581, 0.45771432 ),
    vec2( -0.81544232, -0.87912464 ),
    vec2( -0.38277543, 0.27676845 ),
    vec2( 0.97484398, 0.75648379 ),
    vec2( 0.44323325, -0.97511554 ),
    vec2( 0.53742981, -0.47373420 ),
    vec2( -0.26496911, -0.41893023 ),
    vec2( 0.79197514, 0.19090188 ),
    vec2( -0.24188840, 0.99706507 ),
    vec2( -0.81409955, 0.91437590 ),
    vec2( 0.19984126, 0.78641367 ),
    vec2( 0.14383161, -0.14100790 )
);

float frag_distance_to_screenspace_line(vec2 frag_pos, vec2 line_start, vec2 line_dir)
{
    vec2 af = frag_pos - line_start;
    return sqrt(dot(af, af) - dot(line_dir, af));
}

float random(vec3 seed, int i){
    vec4 seed4 = vec4(seed,i);
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}

float shadow_calculation(vec4 pos_ls, float bias, int cascade_idx)
{
    float shadow = 0.0;

    // range [-1, 1]
    vec3 proj_coords = pos_ls.xyz / pos_ls.w;

    // range [0, 1]
    proj_coords = proj_coords * 0.5 + 0.5;

    float closest_depth = texture(u_shadow_texture, vec4(proj_coords.xy, float(cascade_idx), proj_coords.z));
    float current_depth = proj_coords.z;
    if (current_depth > 1.0)
    {
        return 0.0;
    }

    for(int i = 0; i < 4; i++)
    {
        int index = int(16.0 * random(gl_FragCoord.xyy, i)) % 16;
        shadow += 0.2 * (1.0 - texture(u_shadow_texture, vec4(proj_coords.xy + (poisson_disk[i] / 1000.0) * 0.4, float(cascade_idx), proj_coords.z)));
    }
    return shadow;
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
    return mix(u_solid_color, u_background_color, direction.y * 0.5 + 0.5);
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

    vec3 irradiance = mix(u_solid_color, u_background_color, n.y * 0.5 + 0.5);
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
    vec2 uv = gl_FragCoord.xy / vec2(u_viewport_width, u_viewport_height);
    if(!u_visible || !(u_solid || u_grid))
    {
        discard;
    }


    vec3 light_color = u_light_color;
    vec3 n = normalize(v_normal);
    vec3 l = normalize(u_light_pos);

    float shadow = 0.0;
    if (u_draw_shadows)
    {
        // shadow calculation
        // calculate cascade level
        int cascade_idx = -1;
        int cascade_level = 1;
        if(u_cascade_level < MAX_CASCADE_LEVEL)
        {
            cascade_level = u_cascade_level;
        }
        else
        {
            cascade_level = MAX_CASCADE_LEVEL;
        }
        for(int i = 0; i < cascade_level ; ++i)
        {
            if(v_clipspace_z <= u_cascade_ends[i])
            {
                cascade_idx = i;
                i = cascade_level;
                break;
            }
        }
        if(cascade_idx == -1)
        {
            cascade_idx = u_cascade_level;
        }

        // calculate bias (depending on cascade level)
        float bias = max(u_bias_max * (1.0f - max(0.0, dot(n, l))), u_bias_min);

        //bias *= 1.0 / (u_cascade_ends[cascade_level] * u_bias_modifier);

        //cascade_idx = 0;
        shadow = shadow_calculation(v_pos_ls[cascade_idx], bias, cascade_idx);
    }

    float ao_factor = 1.0;
    if(u_draw_ao)
    {
        ao_factor = texture(u_ssao_texture, uv).r;
    }

    if (u_shadow_only)
    {
        if (shadow == 0.0 && ao_factor == 1.0)
        {
            discard;
        }
        vec3 out_color = vec3(0.25, 0.25, 0.25);
        if (shadow != 0.0)
        {
            FragColor = vec4(out_color * ao_factor, shadow * 0.25);
        }
        else
        {
            FragColor = vec4(out_color, 1.0 - ao_factor);
        }
        return;
    }

    float alpha = 1.0;
    vec3 color = vec3(0.0, 0.0, 0.0);

    vec2 coord = v_uv * float(u_tile_count);

    vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord) * 0.8;
    float line = min(grid.x, grid.y);

    float col = min(line, 1.0);
    col = pow(col, 1.0 / 1.2);

    if(dot((u_cam_pos - v_pos), v_normal) <= 0.0 )
    {
        shadow = 0.0;
        ao_factor = 1.0;
    }

    if(u_grid && u_solid)
    {
        color =  (1.0 - col) * u_grid_color + col * u_solid_color;
    }
    else if(u_grid && !u_solid)
    {
        color = u_grid_color;
        color = color - col * vec3(1.0 - color.x, 1.0 - color.y, 1.0 - color.z);
        alpha = 1.0 - col;
        if(col == 1.0)
        {
            discard;
        }
    }
    else if(!u_grid && u_solid)
    {
        color = u_solid_color;
    }

    vec3 result = color;
    if (u_use_pbr)
    {
        vec3 v = normalize(u_cam_pos - v_pos);
        vec3 light = normalize(u_light_pos - v_pos);
        result = calculate_pbr_lighting(color, n, light, v, ao_factor, shadow);
    }
    else
    {
        result *= 1.0 - shadow * 0.5;
    }

    FragColor = vec4(result, alpha);
}