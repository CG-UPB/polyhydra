#version 330 core

in vec3 v_normal;
flat in int v_visible;
in vec3 v_pos;

uniform vec3 u_cam_pos;

// uniforms for bezier meshes
uniform bool u_is_bezier_mesh;

layout (location = 0) out vec3 normal;

void main()
{
    if (v_visible == 0)
    {
        discard;
    }

    if(u_is_bezier_mesh && dot(v_normal, normalize(u_cam_pos -  v_pos)) < 0)
    {
        normal = -v_normal;
    }
    else
    {
        normal = v_normal;
    }
}