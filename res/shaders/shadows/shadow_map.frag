#version 330 core

in vec3 v_pos;
flat in int v_visible;
in vec3 v_tri_dist;

uniform bool u_draw_wireframe;

uniform int u_viewport_width;
uniform int u_viewport_height;

void main()
{

    if (u_draw_wireframe)
    {
        if (v_visible == 0)
        {
            discard;
        }
        float min_dist_to_edge = min(min(v_tri_dist.x, v_tri_dist.y), v_tri_dist.z);
        float max_dist_to_edge = max(max(v_tri_dist.x, v_tri_dist.y), v_tri_dist.z);
        if (min_dist_to_edge > 0.0015)
        {
            discard;
        }
        if (max_dist_to_edge > 2.0 && (min_dist_to_edge == v_tri_dist.x || min_dist_to_edge == v_tri_dist.z))
        {
            discard;
        }
        return;
    }

    // if face is not visible or transparent: Discard fragment
    // Transparency gets handled in another pass
    if (v_visible == 0 )
    {
        discard;
    }

}