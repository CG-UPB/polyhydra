#version 400 core

layout(vertices = 3) out;

in vec3 v_pos[];
in vec3 v_normal[];
in vec2 v_uv[];
in mat4 v_LightSpacePos0[];
in mat4 v_LightSpacePos1[];

out vec3 tc_pos[];
out vec3 tc_normal[];
out vec2 tc_uv[];
out mat4 tc_LightSpacePos0[];
out mat4 tc_LightSpacePos1[];
out int id[];

void main()
{
    int ID = gl_InvocationID;
//    if(sourceID < 3)
//    {
//        sourceID = (sourceID+1)%3;
//    }

    tc_pos[gl_InvocationID] = v_pos[ID];
    tc_normal[gl_InvocationID] = v_normal[ID];
    tc_uv[gl_InvocationID] = v_uv[ID];
    tc_LightSpacePos0[gl_InvocationID] = v_LightSpacePos0[ID];
    tc_LightSpacePos1[gl_InvocationID] = v_LightSpacePos1[ID];
    id[gl_InvocationID] = ID;

    if (ID == 0)
    {
        gl_TessLevelInner[0] = 1;
        gl_TessLevelOuter[0] = 1;
        gl_TessLevelOuter[1] = 1;
        gl_TessLevelOuter[2] = 1;
    }
}