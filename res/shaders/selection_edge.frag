#version 330 core

flat in int v_instance_id;

uniform int u_selection_offset;
uniform bool u_debug_mode;

out vec4 fragment_color;

ivec4 get_id_bits(int id)
{
    // the first 2 bits are reserved for vertex (01), edge (10) and face (11)
    id = (id << 2) + 2;
    int r = (id & 0x000000FF) >>  0;
    int g = (id & 0x0000FF00) >>  8;
    int b = (id & 0x00FF0000) >> 16;
    int a = (id & 0xFF000000) >> 24;
    return ivec4(r, g, b, a);
}

void main()
{
    ivec4 id_color = get_id_bits(v_instance_id + u_selection_offset);
    fragment_color = vec4(float(id_color.r) / 255.0, float(id_color.g) / 255.0, float(id_color.b) / 255.0, float(id_color.a) / 255.0);
    if (u_debug_mode)
    {
        fragment_color.a = 1.0;
    }
}