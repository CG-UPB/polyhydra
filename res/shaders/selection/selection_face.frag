#version 330 core

flat in int te_visible;

uniform int u_selection_offset;
uniform bool u_debug_mode;
uniform bool u_faces_selectable;

out vec4 fragment_color;

ivec4 get_id_bits(int id)
{
    // the first 2 bits are reserved for vertex (01), edge (10) and face (11)
    id = (id << 2) + 3;
    int r = (id & 0x000000FF) >>  0;
    int g = (id & 0x0000FF00) >>  8;
    int b = (id & 0x00FF0000) >> 16;
    int a = (id & 0xFF000000) >> 24;
    return ivec4(r, g, b, a);
}

void main()
{
    if (te_visible == 0)
    {
        discard;
    }

    int id = u_selection_offset + gl_PrimitiveID;
    ivec4 id_color = get_id_bits(id);
    fragment_color = vec4(float(id_color.r) / 255.0, float(id_color.g) / 255.0, float(id_color.b) / 255.0, float(id_color.a) / 255.0);
    if (u_debug_mode)
    {
        fragment_color.a = 1.0;
    }

    if (!u_faces_selectable)
    {
        fragment_color = vec4(0.0,0.0,0.0,1.0);
    }
}