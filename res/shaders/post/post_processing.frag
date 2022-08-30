#version 330 core

in vec2 v_uv;

uniform bool u_active;

uniform float u_saturation;
uniform float u_contrast;

uniform float u_gamma;
uniform sampler2D u_image;
uniform sampler2D u_depth;

uniform float u_fog_density;
uniform vec3 u_fog_color;
uniform vec3 u_light_color;
uniform vec3 u_light_dir;
uniform vec3 u_cam_pos;
uniform mat4 u_inv_projection;

out vec4 FragColor;

vec3 saturate(vec3 color, float saturation) {
    vec3 luminance = vec3(0.3086, 0.6094, 0.0820);
    float oneMinusSat = 1.0 - saturation;

    vec3 red = vec3(luminance.x * oneMinusSat);
    red.r += saturation;

    vec3 green = vec3(luminance.y * oneMinusSat);
    green.g += saturation;

    vec3 blue = vec3(luminance.z * oneMinusSat);
    blue.b += saturation;

    return mat3(red, green, blue) * color;
}

vec3 apply_fog(in vec3 rgb, in float distance, in vec3 view_dir, in vec3 light_dir)
{
    float fog_amount = 1.0 - exp(-distance * u_fog_density * 0.1);
    float light_amount = max(dot(view_dir, light_dir), 0.0);
    vec3 fog_color = mix(u_fog_color, u_light_color, pow(light_amount, 8.0));
    return mix(rgb, u_fog_color, fog_amount);
}

vec4 get_position(vec2 uv)
{
    vec4 point = u_inv_projection * vec4(uv.xy * 2.0 - 1.0, texture(u_depth, uv).x * 2.0 - 1.0, 1.0);
    return point / point.w;
}

void main()
{
    vec4 color = texture(u_image, v_uv);
    vec4 pos_vs = get_position(v_uv);
    vec3 cam_to_point = pos_vs.xyz - u_cam_pos;
    color.rgb = apply_fog(color.rgb, length(cam_to_point), cam_to_point, -u_light_dir);
    if (u_active)
    {
        color.rgb = ((color.rgb - 0.5f) * max(u_contrast, 0.0)) + 0.5f;
        color.rgb = saturate(color.rgb, u_saturation);
        FragColor = vec4(pow(color.rgb, vec3(1.0 / u_gamma)), color.a);
    }
    else
    {
        FragColor = color;
    }
}