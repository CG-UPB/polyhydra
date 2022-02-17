#version 330 core

out vec4 FragColor;

in vec3 v_pos;
in vec3 v_normal;
in vec4 v_color;
flat in int v_visible;

uniform vec3 u_lightPos;
uniform vec3 u_camPos;
uniform vec3 u_lightColor;
uniform vec4 u_objectColor;
uniform float u_spec_strength;
uniform float u_ambient_strength;
uniform float u_diffuse_strength;
uniform float u_spec_exponent;

uniform int u_viewport_width;
uniform int u_viewport_height;

uniform sampler2D u_color1_texture;
uniform sampler2D u_depth_texture;
uniform sampler2D u_color0_texture;

float near = 0.1;
float far  = 100.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    // if face is not visible or transparent: Discard fragment
    // Transparency gets handled in another pass
    if (v_visible == 0 || u_objectColor.a != 1.0)
    {
        discard;
    }

    //ambient
    float ambientStrength = u_ambient_strength;
    vec3 ambient = ambientStrength * u_lightColor;

    // Phong Shading

    vec3 used_color = mix(u_objectColor, vec3(v_color.x,v_color.y,v_color.z), v_color.w);

    //diffuse
    vec3 n = normalize(v_normal);
    vec3 l = normalize(u_lightPos - v_pos);
    float diff = max(0.0, dot(l, n)) * u_diffuse_strength;
    vec3 diffuse = diff * u_lightColor;

    //specular
    float specularStrength = u_spec_strength;
    vec3 v = normalize(u_camPos - v_pos);
    vec3 r = reflect(-l, n);
    float spec = pow(max(0.0, dot(v, r)), u_spec_exponent);
    vec3 specular = specularStrength * spec * u_lightColor;

    vec3 result = (ambient + diffuse + specular) * used_color;

    FragColor = vec4(result, 1.0);
}
