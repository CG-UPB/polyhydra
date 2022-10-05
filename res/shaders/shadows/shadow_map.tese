#version 400 core

layout(triangles, equal_spacing, ccw) in;

in vec4 tc_Pos[];
flat in int tc_Visible[];
flat in int tc_ovm_halfface_id[];
flat in vec3 tc_center[];

flat out int v_Visible;

// necessary for calculating bezier mesh face normals
uniform mat4 u_light_space_matrices[16];
uniform mat4 u_light_projection;
uniform mat4 u_transform;
uniform float u_cell_size;
// uniform mat4 u_view;
// uniform vec3 u_cam_pos;

// uniform bool u_draw_wireframe;

// uniforms for bezier meshes
uniform bool u_is_bezier_mesh;
uniform int u_bezier_degree;

// use texture buffer for control points
uniform samplerBuffer u_control_points_tb;

// a maximum bezier degree needs to be set because the De Casteljau algorithm 
// needs an array and GLSL only supports local arrays with static sizes
#define MAX_BEZIER_DEGREE 7
#define MAX_CPS_PER_TRI (MAX_BEZIER_DEGREE+1)*(MAX_BEZIER_DEGREE+2)/2

#define CP_2D_INDEX_TO_1D(i2, i1, m) (((m)+1)*((m)+2)/2 - ((m)-(i2)+1)*((m)-(i2)+2)/2 + (i1))

void de_casteljau_2(inout vec3 control_points[MAX_CPS_PER_TRI], int m, float x, float y, float z, int steps)
{
    // Analogous to the De Casteljau algorithm for bezier curves evaluate the 
    // bezier triangle of the current face.
    int cur_m = m;
    for(int s = m; s > m-steps; s--) 
    {
        for(int i2 = 0; i2 < cur_m; i2++) 
        {
            for(int i1 = 0; i1 < cur_m-i2; i1++) 
            {
                int next_i = CP_2D_INDEX_TO_1D(i2, i1, cur_m-1);

                int i_1D = CP_2D_INDEX_TO_1D(i2,   i1,   cur_m);
                control_points[next_i] =  x*control_points[i_1D];

                i_1D = CP_2D_INDEX_TO_1D(i2+1,   i1,   cur_m);
                control_points[next_i] += y*control_points[i_1D];

                i_1D = CP_2D_INDEX_TO_1D(i2,   i1+1,   cur_m);
                control_points[next_i] += z*control_points[i_1D];
            }
        }
        cur_m--;
    }
}

void copy_control_points(inout vec3 dest[MAX_CPS_PER_TRI], int control_points_offset, int m)
{
    // Copy control points into dest which holds the temporary values for the
    // de Casteljau algorithm.
    int num_tri_cps = (m+2)*(m+1)/2;
    for(int i = 0; i < num_tri_cps; i++) {
        dest[i] = texelFetch(u_control_points_tb, control_points_offset + i).xyz;
    }
}

void main()
{
    // barycentric coordinates of current triangle
    float x = gl_TessCoord.x;
    float y = gl_TessCoord.y;
    float z = gl_TessCoord.z;

    if(u_is_bezier_mesh)
    {
        // if the mesh is a bezier mesh evaluate the bezier triangle of the
        // current face for the given barycentric coordinates (x, y, z)
        

        // m is the bezier mesh degree (often used in papers)
        int m = u_bezier_degree;
        int ovm_hf_id = tc_ovm_halfface_id[0];
        int control_points_offset = (ovm_hf_id/2)*(m+2)*(m+1)/2;
        vec3 control_points[MAX_CPS_PER_TRI];
        
        // copy control points from texture buffer into local array 
        // so that it can be modified by the de casteljau algorithm
        copy_control_points(control_points, control_points_offset, m);

        // execute the de casteljau algorithm
        de_casteljau_2(control_points, m, x, y, z, m);
        
        // mat4 view_transform = inverse(u_light_projection) * u_light_space_matrices[0] * u_transform;
        // Perform Cell sizing.
        control_points[0] = tc_center[0] + (control_points[0] - tc_center[0]) * u_cell_size;
        
        gl_Position = u_transform * vec4(control_points[0], 1.0);
    }
    else
    {
        gl_Position   = tc_Pos[0]            *x + tc_Pos[1]            *y + tc_Pos[2]            *z;
    }

    // in any case use the values of vertex shader for these values 
    v_Visible         = tc_Visible[1]        ; // flat
}