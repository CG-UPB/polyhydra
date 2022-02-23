#version 330 core

in vec3 v_pos;
in vec3 v_normal;
in vec4 v_color;
in vec4 v_pos_ls;
flat in int v_visible;

uniform vec3 u_lightPos;
uniform vec3 u_camPos;
uniform vec3 u_lightColor;
uniform vec4 u_objectColor;

uniform int u_viewport_width;
uniform int u_viewport_height;

uniform sampler2D u_depth_texture;
uniform sampler2D u_ssao_texture;
uniform sampler2D u_shadow_texture;

out vec4 FragColor;

float ShadowCalculation(vec4 pos_ls, float bias)
{
    // range [-1, 1]
    vec3 proj_coords = pos_ls.xyz / pos_ls.w;

    // range [0, 1]
    proj_coords = proj_coords * 0.5 + 0.5;

    float closest_depth = texelFetch(u_shadow_texture, ivec2(proj_coords.xy), 0).r;
    float current_depth = proj_coords.z;
    float shadow = current_depth - bias > closest_depth ? 1.0 : 0.0;

    return shadow;
}

void main()
{

    // if face is not visible or transparent: Discard fragment
    // Transparency gets handled in another pass
    if (v_visible == 0 || u_objectColor.a != 1.0)
    {
        discard;
    }

    vec2 uv = gl_FragCoord.xy / vec2(u_viewport_width, u_viewport_height);

    //ambient
    float ambientStrength = 1.0;
    float ao_factor = texture(u_ssao_texture, uv).r;
    vec3 ambient = ambientStrength * u_lightColor * ao_factor;

    // Phong Shading

    vec3 used_color = mix(u_objectColor.rgb, vec3(v_color.x,v_color.y,v_color.z), v_color.w);

    //diffuse
    float diffuseStrength = 1.0;
    vec3 n = -normalize(v_normal);
    //vec3 l = normalize(u_lightPos - v_pos);
    // constant light direction looks way better than a single point of light
    vec3 l = normalize(vec3(0.0, -1.0, -1.0));
    float diff = max(0.0, dot(l, n));
    vec3 diffuse = diffuseStrength * diff * u_lightColor;

    //specular
    float specularStrength = 0.2;
    vec3 v = normalize(u_camPos - v_pos);
    vec3 r = reflect(-l, n);
    float spec = pow(max(0.0, dot(v, r)), 8);
    vec3 specular = specularStrength * spec * u_lightColor;

    vec3 light_dir = normalize(u_lightPos - v_pos);
    float bias = max(0.05 * (1.0 - dot(n, light_dir)), 0.005);
    float shadow = ShadowCalculation(v_pos_ls, bias);

    float norm = ambientStrength + diffuseStrength + specularStrength;
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) / norm * used_color;

    FragColor = vec4(result, 1.0);
//
//    vec4 pos_ls = v_pos_ls;
//    vec3 proj_coords = pos_ls.xyz / pos_ls.w;
//    proj_coords = proj_coords * 0.5 + 0.5;
//    float depth = texelFetch(u_shadow_texture, ivec2(proj_coords.xy), 0).r;
//    FragColor = vec4(vec3(depth), 1.0);
}