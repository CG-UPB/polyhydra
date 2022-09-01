#version 330 core

const int MAX_CASCADE_LEVEL = 8;

in vec2 v_uv;

uniform sampler2DArrayShadow u_shadow_texture;
uniform sampler2D u_depth;
uniform mat4 u_inv_projection;
uniform int u_cascade_level;

uniform mat4 u_light_projection[MAX_CASCADE_LEVEL];
uniform mat4 u_light_view[MAX_CASCADE_LEVEL];

out vec4 FragColor;

vec4 get_position(vec2 uv)
{
    vec4 point = u_inv_projection * vec4(uv.xy * 2.0 - 1.0, texture(u_depth, uv).x * 2.0 - 1.0, 1.0);
    return point / point.w;
}

void main()
{
    int n = u_cascade_level;
    vec3 frag_pos_vs = get_position(v_uv).xyz;
    vec4 frag_pos_ls = u_light_projection[n] * u_light_view[n] * vec4(frag_pos_vs.xyz, 1.0);
    // range [-1, 1]
    vec3 proj_coords = frag_pos_ls.xyz / frag_pos_ls.w;

    // range [0, 1]
    proj_coords = proj_coords * 0.5 + 0.5;
    if (proj_coords.z > 0.0)
    {
        float depth = 1.0 - texture(u_shadow_texture, vec4(proj_coords.xy, float(u_cascade_level), proj_coords.z));
        FragColor = vec4(depth, depth, depth, 1.0);
    }
    else
    {
        discard;
    }
}