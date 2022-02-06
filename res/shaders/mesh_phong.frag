#version 330 core

layout(location = 0) out vec4 FragColor;

in vec3 v_pos;
in vec3 v_normal;
flat in int v_visible;

uniform vec3 u_lightPos;
uniform vec3 u_camPos;
uniform vec3 u_lightColor;
uniform vec3 u_objectColor;

uniform int u_viewport_width;
uniform int u_viewport_height;

uniform sampler2D u_color1_texture;
uniform sampler2D u_depth_texture;
uniform sampler2D u_color0_texture;

float near = 0.1;
float far  = 10.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{

    if (v_visible == 0)
    {
        discard;
    }
    //ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * u_lightColor;

    // Phong Shading

    //diffuse
    vec3 n = normalize(v_normal);
    vec3 l = normalize(u_lightPos - v_pos);
    float diff = max(0.0, dot(l, n));
    vec3 diffuse = diff * u_lightColor;

    //specular
    float specularStrength = 0.5;
    vec3 v = normalize(u_camPos - v_pos);
    vec3 r = reflect(-l, n);
    float spec = pow(max(0.0, dot(v, r)), 8);
    vec3 specular = specularStrength * spec * u_lightColor;

    vec3 result = (ambient + diffuse + specular) * u_objectColor;

    FragColor = vec4(result, 1.0);

    // Testing
//    vec2 uv = gl_FragCoord.xy / vec2(u_viewport_width, u_viewport_height);
////    float depth = LinearizeDepth(texture(u_depth_texture, res).x) / far; // divide by far for demonstration
////    FragColor = vec4(vec3(depth), 1.0);
//
//    FragColor = texture(u_color1_texture, uv);
}