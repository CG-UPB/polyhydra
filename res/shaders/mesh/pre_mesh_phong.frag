#version 330 core

in vec3 v_normal;
flat in int v_visible;
in vec3 v_pos;

uniform vec3 u_cam_pos;

// uniforms for bezier meshes
uniform bool u_is_bezier_mesh;
uniform bool u_two_sided_lighting;

layout (location = 0) out vec3 normal;

void main()
{
    if (v_visible == 0)
    {
        discard;
    }

    vec3 n = normalize(v_normal);
    vec3 v = normalize(u_cam_pos - v_pos);

    if(u_is_bezier_mesh && dot(n, v) < 0)
    {
        n = -n;
    }
    else if(u_two_sided_lighting && dot(n, v) < 0 )
    {
        n = -n;
    }
    else
    {
        normal = n;
    }
}