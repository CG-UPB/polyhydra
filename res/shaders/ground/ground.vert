#version 330 core

const int MAX_CASCADE_LEVEL = 8;
const int HALF_MAX_CASCADE_LEVEL = MAX_CASCADE_LEVEL / 2;

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;


out vec3 v_Pos;
out vec3 v_Normal;
out vec2 v_UV;
out vec4 v_Color;
//out float v_clipspace_z;

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

    v_Pos = vec3(u_transform * vec4(pos, 1.0));
    gl_Position = u_projection * u_view * vec4(v_Pos, 1.0);
    v_Normal = mat3(transpose(inverse(u_transform))) * a_normal;
    v_UV = a_uv;
    v_Color = vec4(1.0);

    // Cascaded Shadowmap (loops do not work here, we need to unroll the loop to compile this)
//    mat4 light_space_mat = u_light_projection[0] * u_light_view[0] * u_light_transform;
//    v_LightSpacePos0[0] = light_space_mat * vec4(pos, 1.0);
//    light_space_mat = u_light_projection[1] * u_light_view[1] * u_light_transform;
//    v_LightSpacePos0[1] = light_space_mat * vec4(pos, 1.0);
//    light_space_mat = u_light_projection[2] * u_light_view[2] * u_light_transform;
//    v_LightSpacePos0[2] = light_space_mat * vec4(pos, 1.0);
//    light_space_mat = u_light_projection[3] * u_light_view[3] * u_light_transform;
//    v_LightSpacePos0[3] = light_space_mat * vec4(pos, 1.0);
//    light_space_mat = u_light_projection[4] * u_light_view[4] * u_light_transform;
//    v_LightSpacePos1[0] = light_space_mat * vec4(pos, 1.0);
//    light_space_mat = u_light_projection[5] * u_light_view[5] * u_light_transform;
//    v_LightSpacePos1[1] = light_space_mat * vec4(pos, 1.0);
//    light_space_mat = u_light_projection[6] * u_light_view[6] * u_light_transform;
//    v_LightSpacePos1[2] = light_space_mat * vec4(pos, 1.0);
//    light_space_mat = u_light_projection[7] * u_light_view[7] * u_light_transform;
//    v_LightSpacePos1[3] = light_space_mat * vec4(pos, 1.0);

}