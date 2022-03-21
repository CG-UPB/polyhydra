#version 330 core

layout (location = 0) out vec4 FragColor;

in vec3 v_pos;
flat in int v_visible;
in vec3 v_tri_dist;
flat in int v_is_triangle;
flat in vec4 v_a_adir;
flat in vec4 v_b_bdir;
flat in int v_use_lookup_path;


uniform bool u_draw_wireframe;
uniform float u_wireframe_size;

uniform int u_viewport_width;
uniform int u_viewport_height;

float LinearizeDepth(float depth)
{
    float near = 0.1;
    float far = 100.0;
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

float frag_distance_to_screenspace_line(vec2 frag_pos, vec2 line_start, vec2 line_dir)
{
    vec2 af = frag_pos - line_start;
    return sqrt(dot(af, af) - dot(line_dir, af));
}

void main()
{

    vec2 uv = gl_FragCoord.xy / vec2(u_viewport_width, u_viewport_height);
    if (u_draw_wireframe)
    {
        if (v_visible == 0)
        {
            discard;
        }
        float size_factor = 0.0015 * u_wireframe_size;
        if (v_use_lookup_path == 1)
        {
            // these triangles are very likely not visible, since we don't draw 2/3rds of those anyway
            if (v_is_triangle == 0)
            {
                discard;
            }
            else
            {
                float dist0 = frag_distance_to_screenspace_line(uv, v_a_adir.xy, v_a_adir.zw);
                float dist1 = frag_distance_to_screenspace_line(uv, v_b_bdir.xy, v_b_bdir.zw);
                if (v_a_adir.xy != v_b_bdir.xy)
                {
                    float dist2 = frag_distance_to_screenspace_line(uv, v_a_adir.xy, normalize(v_b_bdir.xy - v_a_adir.xy));
                    float min_dist_to_edge = min(min(dist0, dist1), dist2);
                    if (min_dist_to_edge > size_factor)
                    {
                        discard;
                    }
                }
                else
                {
                    float min_dist_to_edge = min(dist0, dist1);
                    if (min_dist_to_edge > size_factor)
                    {
                        discard;
                    }
                }
            }
        }
        else
        {
            float min_dist_to_edge = min(min(v_tri_dist.x, v_tri_dist.y), v_tri_dist.z);
            if (min_dist_to_edge > size_factor)
            {
                discard;
            }
            // here, we discard 2 of our 3 edges that we added in our triangulation, since only want to draw the original edges
            if (v_is_triangle == 0 && (min_dist_to_edge == v_tri_dist.x || min_dist_to_edge == v_tri_dist.z) && v_tri_dist.y > size_factor)
            {
                discard;
            }
        }
        return;
    }

    // if face is not visible or transparent: Discard fragment
    // Transparency gets handled in another pass
    if (v_visible == 0 )
    {
        discard;
    }
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
    //FragColor = vec4(0.4, 0.9, 0.1, 1.0);

}