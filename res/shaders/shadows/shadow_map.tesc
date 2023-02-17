#version 400 core

layout(vertices = 3) out;

in vec3 v_Pos[];
in vec2 v_UV[];

flat in float v_min_edge_length[];
in vec4 v_rounding_sphere_center[];
in vec3 v_center[];
flat in int v_Visible[];
flat in int v_isTriangle[];
flat in float v_VertexTypeRounded[];
flat in int v_ovm_halfface_id[];

out vec3 tc_Pos[];

flat out float tc_min_edge_length[];
out vec4 tc_rounding_sphere_center[];
out vec3 tc_center[];

flat out int tc_isTriangle[];
flat out int tc_Visible[];
flat out float tc_VertexTypeRounded[];
flat out int tc_ovm_halfface_id[];

uniform bool u_rounding;
uniform bool u_draw_wireframe;
uniform bool u_is_bezier_mesh;
uniform int u_bezier_tessellation_level;

#define ID gl_InvocationID

void main()
{
    int sourceID = ID;
    if(u_draw_wireframe && u_is_bezier_mesh && sourceID < 3)
    {
        sourceID = (sourceID+1)%3;
    }

    tc_Pos[ID]             =  v_Pos[sourceID];
    tc_isTriangle[ID]      =  v_isTriangle[sourceID];
    tc_min_edge_length[ID] =  v_min_edge_length[sourceID];
    tc_rounding_sphere_center[ID] = v_rounding_sphere_center[sourceID];
    tc_center[ID]          =  v_center[sourceID];
    tc_Visible[ID]         =         v_Visible[sourceID];
    tc_VertexTypeRounded[ID] =  v_VertexTypeRounded[sourceID];
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