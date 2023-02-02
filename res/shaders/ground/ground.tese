#version 400 core
layout(triangles, equal_spacing, ccw) in;

const int MAX_CASCADE_LEVEL = 8;


in vec3 tc_pos[];
in vec3 tc_normal[];
in vec2 tc_uv[];
in mat4 tc_LightSpacePos0[];
in mat4 tc_LightSpacePos1[];
in int id[];

uniform mat4 u_transform;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 v_pos;
out vec3 v_normal;
out vec2 v_uv;
out vec4 v_pos_ls[MAX_CASCADE_LEVEL];
out float v_clipspace_z;


void set_light_space_pos(int vertex_index)
{
    v_pos_ls[0] = tc_LightSpacePos0[vertex_index][0];
    v_pos_ls[1] = tc_LightSpacePos0[vertex_index][1];
    v_pos_ls[2] = tc_LightSpacePos0[vertex_index][2];
    v_pos_ls[3] = tc_LightSpacePos0[vertex_index][3];
    v_pos_ls[4] = tc_LightSpacePos1[vertex_index][0];
    v_pos_ls[5] = tc_LightSpacePos1[vertex_index][1];
    v_pos_ls[6] = tc_LightSpacePos1[vertex_index][2];
    v_pos_ls[7] = tc_LightSpacePos1[vertex_index][3];
}

void main()
{
    float x = gl_TessCoord.x;
    float y = gl_TessCoord.y;
    float z = gl_TessCoord.z;

    v_pos = tc_pos[0] * x + tc_pos[1] * y + tc_pos[2] * z;
    v_normal = tc_normal[0] * x + tc_normal[1] * y + tc_normal[2] * z;
    v_uv = tc_uv[0] * x + tc_uv[1] * y + tc_uv[2] * z;
    if(x > 0.0)
    {
        set_light_space_pos(0);
    }
    else if(y > 0.0)
    {
        set_light_space_pos(1);

    }
    else if(z > 0.0)
    {
        set_light_space_pos(2);
    }

    vec4 screen_pos = u_projection * u_view * vec4(v_pos, 1.0);
    v_clipspace_z = screen_pos.z;
    gl_Position = screen_pos;

}