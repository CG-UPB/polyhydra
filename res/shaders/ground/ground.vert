#version 330 core

const int MAX_CASCADE_LEVEL = 8;
const int HALF_MAX_CASCADE_LEVEL = MAX_CASCADE_LEVEL / 2;

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;


out vec3 v_pos;
out vec3 v_normal;
out vec2 v_uv;

uniform mat4 u_transform;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_light_projection[MAX_CASCADE_LEVEL];
uniform mat4 u_light_view[MAX_CASCADE_LEVEL];
uniform mat4 u_light_transform;

uniform vec3 u_light_pos;
uniform vec3 u_cam_pos;
uniform vec3 u_light_color;

uniform float u_height;


void main()
{

    mat4 view_transform = u_view * u_transform;

    vec3 pos = a_pos + vec3(0.0, u_height, 0.0);

    v_pos = vec3(u_transform * vec4(pos, 1.0));
    gl_Position = u_projection * u_view * vec4(v_pos, 1.0);
    v_normal = mat3(transpose(inverse(u_transform))) * a_normal;
    v_uv = a_uv;

}