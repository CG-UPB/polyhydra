
// phong lighting model
uniform float u_spec_strength;
uniform float u_ambient_strength;
uniform float u_diffuse_strength;
uniform float u_spec_exponent;

vec3 calculate_phong_lighting(vec3 color, vec3 n, vec3 l, vec3 v, float ao, float shadow, vec3 light_color)
{
    vec3 ambient = u_ambient_strength * light_color * ao;

    //diffuse
    float diff = max(0.0, dot(l, n));
    vec3 diffuse = u_diffuse_strength * diff * light_color;

    //specular
    vec3 r = reflect(-l, n);
    vec3 h = normalize(l + v);
    float spec = pow(max(0.0, dot(h, n)), u_spec_exponent);
    vec3 specular = u_spec_strength * spec * light_color;

    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    return result;
}
