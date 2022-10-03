#version 400 core

layout(vertices = 3) out;

in vec3 v_Pos[];
in vec3 v_Normal[];
in vec4 v_Color[];
in mat4 v_LightSpacePos0[];
in mat4 v_LightSpacePos1[];
in float v_clipspace_z[];
flat in int v_Visible[];
flat in int v_isTriangle[];
flat in int v_ovm_halfface_id[];

out vec3 tc_Pos[];
out vec3 tc_Normal[];
out vec4 tc_Color[];
out mat4 tc_LightSpacePos0[];
out mat4 tc_LightSpacePos1[];
out float tc_clipspace_z[];
flat out int tc_Visible[];
flat out int tc_isTriangle[];
flat out int tc_ovm_halfface_id[];

uniform bool u_draw_wireframe;
uniform bool u_is_bezier_mesh;
uniform int u_bezier_tessellation_level;

#define ID gl_InvocationID

void main()
{
    // For the wireframe mode, the order of vertex data is different
    int sourceID = ID;
    if(u_draw_wireframe && sourceID < 3)
    {
        sourceID = (sourceID+1)%3;
    }

    tc_Pos[ID]             =             v_Pos[sourceID];
    tc_Normal[ID]          =          v_Normal[sourceID];
    tc_Color[ID]           =           v_Color[sourceID];
    tc_LightSpacePos0[ID]  =  v_LightSpacePos0[sourceID];
    tc_LightSpacePos1[ID]  =  v_LightSpacePos1[sourceID];
    tc_clipspace_z[ID]     =     v_clipspace_z[sourceID];
    tc_Visible[ID]         =         v_Visible[sourceID];
    tc_isTriangle[ID]      =      v_isTriangle[sourceID];
    tc_ovm_halfface_id[ID] = v_ovm_halfface_id[sourceID];

    if (ID == 0) 
    {
        // only do tessellation for bezier meshes
        if(u_is_bezier_mesh) 
        {
            if(u_draw_wireframe)
            {
                // in wireframe mode inner triangles are not necessary
                // and gl_TessLevelInner[0] = 1 sometimes yields incorrect outlines
                gl_TessLevelInner[0] = 2;
            }
            else
            {
                gl_TessLevelInner[0] = u_bezier_tessellation_level;
            }
            gl_TessLevelOuter[0] = u_bezier_tessellation_level;
            gl_TessLevelOuter[1] = u_bezier_tessellation_level;
            gl_TessLevelOuter[2] = u_bezier_tessellation_level;
        }
        else
        {
            gl_TessLevelInner[0] = 1;
            gl_TessLevelOuter[0] = 1;
            gl_TessLevelOuter[1] = 1;
            gl_TessLevelOuter[2] = 1;
        }
    }
}