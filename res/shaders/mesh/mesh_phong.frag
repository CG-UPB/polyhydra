#version 330 core

#define PI 3.14159265359

const int MAX_CASCADE_LEVEL = 8;

in vec3 v_pos;
in vec3 v_normal;
in vec4 v_color;
in vec4 v_pos_ls[MAX_CASCADE_LEVEL];
in float v_clipspace_z;
flat in int v_visible;
in vec3 v_tri_dist;
flat in int v_is_triangle;
flat in vec4 v_a_adir;
flat in vec4 v_b_bdir;
flat in int v_use_lookup_path;
flat in int v_tes_inner_tri;

uniform bool u_draw_wireframe;
uniform bool u_draw_shadows;
uniform bool u_draw_ao;
uniform float u_wireframe_size;

uniform vec3 u_light_pos;
uniform float u_light_size = 1.0;
uniform vec3 u_cam_pos;
uniform vec3 u_view_dir;
uniform vec3 u_light_color;
uniform vec4 u_object_color;

uniform int u_viewport_width;
uniform int u_viewport_height;

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

uniform float u_shadow_strength;

uniform float peel_depth;
uniform int u_cascade_level;
uniform float u_cascade_ends[MAX_CASCADE_LEVEL];
uniform mat4 u_view;

uniform float u_bias_min;
uniform float u_bias_max;
uniform float u_bias_modifier;

// uniforms for bezier meshes
uniform bool u_is_bezier_mesh;

uniform sampler2D u_depth_texture;
uniform sampler2D u_ssao_texture;
uniform sampler2D u_transparent_shadow_texture;
uniform sampler2D u_color_filter_texture;
uniform sampler2DArray u_shadow_texture;

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

void draw_wireframe(vec2 uv)
{
    if (v_visible == 0)
    {
        discard;
    }
    if(u_is_bezier_mesh && v_tes_inner_tri == 1)
    {
        // Discard all inner triangles of a tessellated triangle.
        discard;
    }
    float size_factor = 0.0015 * u_wireframe_size;
    if (v_use_lookup_path == 1)
    {
        // these triangles are very likely not visible, since we don't draw 2/3rds of those anyway
        if (v_is_triangle == 0)
        {
            discard;
        }
        else
        {
            float dist0 = frag_distance_to_screenspace_line(uv, v_a_adir.xy, v_a_adir.zw);
            float dist1 = frag_distance_to_screenspace_line(uv, v_b_bdir.xy, v_b_bdir.zw);
            if (v_a_adir.xy != v_b_bdir.xy)
            {
                float dist2 = frag_distance_to_screenspace_line(uv, v_a_adir.xy, normalize(v_b_bdir.xy - v_a_adir.xy));
                float min_dist_to_edge = min(min(dist0, dist1), dist2);
                if (min_dist_to_edge > size_factor)
                {
                    discard;
                }
            }
            else
            {
                float min_dist_to_edge = min(dist0, dist1);
                if (min_dist_to_edge > size_factor)
                {
                    discard;
                }
            }
        }
    }
    else
    {
        float min_dist_to_edge;
        if(!u_is_bezier_mesh)
        {
            min_dist_to_edge = min(min(v_tri_dist.x, v_tri_dist.y), v_tri_dist.z);
        }
        else
        {
            // For Bézier meshes, draw an outline only for outer edges.
            min_dist_to_edge = v_tri_dist.y;
        }

        if (min_dist_to_edge > size_factor)
        {
            discard;
        }
        // here, we discard 2 of our 3 edges that we added in our triangulation, since only want to draw the original edges
        if (v_is_triangle == 0 && (min_dist_to_edge == v_tri_dist.x || min_dist_to_edge == v_tri_dist.z) && v_tri_dist.y > size_factor)
        {
            discard;
        }
    }
    FragColor = vec4(u_object_color.rgb, 1.0);
}

void draw_wireframe_ontop(vec2 uv)
{
    bool fragment_in_wireframe = true;
    float size_factor = 0.0015 * u_wireframe_size;
    if (v_use_lookup_path == 1)
    {
        // these triangles are very likely not visible, since we don't draw 2/3rds of those anyway
        if (v_is_triangle == 0)
        {
            fragment_in_wireframe = false;
        }
        else
        {
            float dist0 = frag_distance_to_screenspace_line(uv, v_a_adir.xy, v_a_adir.zw);
            float dist1 = frag_distance_to_screenspace_line(uv, v_b_bdir.xy, v_b_bdir.zw);
            if (v_a_adir.xy != v_b_bdir.xy)
            {
                float dist2 = frag_distance_to_screenspace_line(uv, v_a_adir.xy, normalize(v_b_bdir.xy - v_a_adir.xy));
                float min_dist_to_edge = min(min(dist0, dist1), dist2);
                if (min_dist_to_edge > size_factor)
                {
                    fragment_in_wireframe = false;
                }
            }
            else
            {
                float min_dist_to_edge = min(dist0, dist1);
                if (min_dist_to_edge > size_factor)
                {
                    fragment_in_wireframe = false;
                }
            }
        }
    }
    else
    {
        float min_dist_to_edge = min(min(v_tri_dist.x, v_tri_dist.y), v_tri_dist.z);
        if (min_dist_to_edge > size_factor)
        {
            fragment_in_wireframe = false;
        }
        // here, we discard 2 of our 3 edges that we added in our triangulation, since only want to draw the original edges
        if (v_is_triangle == 0 && (min_dist_to_edge == v_tri_dist.x || min_dist_to_edge == v_tri_dist.z) && v_tri_dist.y > size_factor)
        {
            fragment_in_wireframe = false;
        }
    }
    if (fragment_in_wireframe)
    {
        FragColor = vec4(u_object_color.rgb * 0.5, 1.0);
    }
}

float random(vec3 seed, int i){
    vec4 seed4 = vec4(seed,i);
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}

float get_blocker_distance(vec3 shadow_coords, float bias, float light_size, int cascade_idx)
{
    int blockers = 0;
    float avg_blocker_distance = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(u_shadow_texture, 0));


    float search_width = light_size * (shadow_coords.z - 0.05) / shadow_coords.z;
    if(search_width < 0)
    {
        return 0;
    }

    int range = int(search_width);
    int samples = 3;
    for(int x = -samples; x <= samples; ++x)
    {
        for (int y = -samples; y <= samples; ++y)
        {
            vec2 shift = vec2(x * 2.0  * range / samples, y * 2.0 * range / samples);
            float z = texture(u_shadow_texture, vec3(shadow_coords.xy + vec2(x, y) + shift * texelSize, float(cascade_idx))).r;
            if(z < (shadow_coords.z - bias))
            {
                blockers++;
                avg_blocker_distance += z;
            }
        }
    }

    if(blockers > 0)
    {
        return avg_blocker_distance / blockers;
    }
    else
    {
        return -1.0;
    }

}

float percentage_closer_filtering(vec3 shadow_coords, float radius, float bias, int cascade_idx)
{
    float sum = 0;
    int count = 0;

    vec2 texelSize = 1.0 / vec2(textureSize(u_shadow_texture, 0));
    int range = int(radius * 10.0);
    range = range > 40 ? 40 : range;
    range = range <  1 ?  2 : range;

    for(int x = - range; x <= range; ++x)
    {
        count++;
        for (int y = -range; y <= range; ++y)
        {
            float z = texture(u_shadow_texture, vec3(shadow_coords.xy + vec2(x , y) * texelSize, float(cascade_idx))).r;
            if(z < (shadow_coords.z - bias))
            {
                sum += 1.0;
            }
        }
    }
    count = count > 0 ? count : 1;

    return sum / (count * count);
}

float pcss_shadow_calculation(vec4 pos_ls, float light_size, float bias, int cascade_idx)
{
    vec3 shadow_coords = pos_ls.rgb / pos_ls.w;
    shadow_coords = shadow_coords * 0.5 + 0.5;

    if(shadow_coords.z > 1.0)
    {
        return 0.0;
    }

    //Step 1: Blocker search
    float blocker_distance = get_blocker_distance(shadow_coords, bias, light_size, cascade_idx);
    if(blocker_distance == -1)
    return 0.0;

    //Step 2: Penumbra estimation
    float penumbra_width = light_size * ((shadow_coords.z - blocker_distance) / blocker_distance);
    if(penumbra_width == 0)
    return 1.0;

    //Step 3: Filtering
    float radius = penumbra_width;

    return percentage_closer_filtering(shadow_coords, radius, bias, cascade_idx);
}

float shadow_calculation(vec4 pos_ls, float bias, int cascade_idx)
{
    float shadow = 0.0;

    vec3 n = normalize(v_normal);
    // range [-1, 1]
    vec3 proj_coords = pos_ls.xyz / pos_ls.w;

    // range [0, 1]
    proj_coords = proj_coords * 0.5 + 0.5;

    float current_depth = proj_coords.z - bias;
    float closest_depth = texture(u_shadow_texture, vec3(proj_coords.xy, float(cascade_idx))).r;

    if (current_depth > 1.0)
    {
        return 0.0;
    }

    for(int i = 0; i < 4; i++)
    {
        int index = int(16.0 * random(gl_FragCoord.xyy, i)) % 16;
        //shadow += 0.25 * (1.0 - texture(u_shadow_texture, vec4(proj_coords.xy + (poisson_disk[i] / 1000.0) * 0.4, float(cascade_idx), current_depth)));
    }
    return shadow;
}

float get_shadow(vec3 normal, vec3 light_dir)
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
    float light_angle = clamp(dot(normal, light_dir), 0.0, 1.0);
    //shadow = max(light_angle, 0.0);
    float bias = max(u_bias_max * (1.0f - max(0.0, dot(normal, light_dir))), u_bias_min);

    //return shadow_calculation(v_pos_ls[cascade_idx], bias, cascade_idx);
    return pcss_shadow_calculation(v_pos_ls[cascade_idx], u_light_size, bias, cascade_idx);
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
    vec2 uv = gl_FragCoord.xy / vec2(u_viewport_width, u_viewport_height);
    if (u_draw_wireframe)
    {
        draw_wireframe(uv);
        return;
    }

    // if face is not visible or transparent: Discard fragment
    // Transparency gets handled in another pass
    float alpha = v_color.a;
    if (v_visible == 0 || alpha < 1.0 - 0.00001)
    {
        discard;
    }

#if 0 // show wireframe on top of the mesh, comment out the other wireframe stuff on top or this won't work
    draw_wireframe_ontop(uv);
    //return;
#endif

    vec3 n = normalize(v_normal);
//    if(dot(u_cam_pos - pos, n) <= -0.05)
//    {
//        n = -n;
//    }

    // For Bézier meshes, always use a normal directed towards the camera.
    if(u_is_bezier_mesh && dot(n, normalize(u_cam_pos -  v_pos)) < 0)
    {
        n = -n;
    }

    vec3 l = normalize(u_light_pos);

    float shadow = 0.0;
    if (u_draw_shadows)
    {
        shadow = get_shadow(n, l);
        shadow = shadow * u_shadow_strength;
    }

    // Phong Shading
    vec4 used_color = vec4(1.0f);
    if(v_color.r >= 0.0 && v_color.g >= 0.0 && v_color.b >= 0.0)
    {
        used_color = v_color;
    }
    else
    {
        used_color = u_object_color;
    }

    //ambient
    float ao_factor = 1.0;
    if (u_draw_ao)
    {
        ao_factor = texture(u_ssao_texture, uv).r;
    }

    vec3 result;
    if (u_use_pbr)
    {
        vec3 v = normalize(u_cam_pos - v_pos);
        vec3 light = normalize(u_light_pos);
        result = calculate_pbr_lighting(used_color.rgb, n, light, v, ao_factor, shadow);
    }
    else
    {
        result = calculate_phong_lighting(used_color.rgb, n, l, ao_factor, shadow);
    }
    FragColor = vec4(result, used_color.a);
}