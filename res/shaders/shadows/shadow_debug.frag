#version 330 core

const int MAX_CASCADE_LEVEL = 8;

in vec2 v_uv;

uniform sampler2DArray u_shadow_texture;
uniform sampler2D u_depth;
uniform mat4 u_inv_projection;
uniform int u_cascade_level;

uniform mat4 u_light_projection[MAX_CASCADE_LEVEL];
uniform mat4 u_light_view[MAX_CASCADE_LEVEL];

uniform float u_near;
uniform float u_far;

out vec4 FragColor;

vec4 get_position(vec2 uv)
{
    vec4 point = u_inv_projection * vec4(uv.xy * 2.0 - 1.0, texture(u_depth, uv).x * 2.0 - 1.0, 1.0);
    return point / point.w;
}

float linearize_depth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * u_near * u_far) / (u_far + u_near - z * (u_far - u_near));
}

void main()
{
    float depth = texture(u_shadow_texture, vec3(v_uv, float(u_cascade_level))).r;
    depth = linearize_depth(depth);
    FragColor = vec4(depth, depth, depth, 1.0);
}