#version 400 core

layout(vertices = 3) out;

in vec4 v_Pos[];
in vec3 v_normal[];
flat in int v_visible[];
flat in int v_ovm_halfface_id[];

out vec4 tc_Pos[];
out vec3 tc_normal[];
flat out int tc_visible[];
flat out int tc_ovm_halfface_id[];

uniform bool u_is_bezier_mesh;
uniform int u_bezier_tessellation_level;

#define ID gl_InvocationID

void main()
{
    tc_Pos[ID]             =             v_Pos[ID];
    tc_normal[ID]          =          v_normal[ID];
    tc_visible[ID]         =         v_visible[ID];
    tc_ovm_halfface_id[ID] = v_ovm_halfface_id[ID];

    if (ID == 0) 
    {
        // only do tessellation for bezier meshes
        if(u_is_bezier_mesh) 
        {
            gl_TessLevelInner[0] = u_bezier_tessellation_level;
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