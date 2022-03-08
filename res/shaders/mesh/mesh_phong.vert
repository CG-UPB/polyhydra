#version 330 core

#define PI 3.14159265358979323846

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Center;
layout (location = 3) in float a_peel_depth;
layout (location = 4) in float a_isDigged;
layout (location = 5) in vec4 a_Color;
layout (location = 6) in float a_isIsolated;
layout (location = 7) in float a_isTriangle;
layout (location = 8) in float a_rounded_vertex_type;
layout (location = 9) in vec4 a_rounded_face_center_or_to_vertex;
layout (location = 10) in float a_isSelected;

out vec3 v_Pos;
out vec3 v_Normal;
out vec4 v_Color;
out vec4 v_LightSpacePos;
flat out int v_Visible;
flat out int v_isTriangle;

uniform bool u_rounding;
uniform float u_rounding_size;

uniform mat4 u_Transform;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform vec3 u_lightPos;
uniform vec3 u_camPos;
uniform vec3 u_lightColor;
uniform vec4 u_objectColor;
uniform float u_cell_size;
uniform vec4 u_selection_color;
uniform float u_average_cell_size;

uniform mat4 u_light_projection;
uniform mat4 u_light_view;
uniform mat4 u_light_transform;

uniform int u_peel_depth;
uniform float u_slice_depth;
uniform vec3 u_min;
uniform vec3 u_max;
uniform vec3 u_slice_direction;
uniform bool u_slice_locked;

const float ROUNDED_VERTEX_TYPE_FACE     = 0.0;
const float ROUNDED_VERTEX_TYPE_EDGE     = 1.0;
const float ROUNDED_VERTEX_TYPE_CORNER   = 2.0;
const float ROUNDED_VERTEX_TYPE_CENTER   = 3.0;

const float EDGE_FACTOR = 1.0 / sqrt(2.0);
const float CORNER_FACTOR = sqrt(2.0);

float get_shrink_factor(float angle, float dist) {
    float half_angle = angle * 0.5;
    return dist * (1.0 / cos(half_angle) - tan(half_angle));
}

float get_dist(float inner_angle, float r) {
    return r;
    return cos(inner_angle * 0.5) * r;
}

void main()
{
    ////////////////////////////////////////////////////////
    // Slicing and Peeling
    ////////////////////////////////////////////////////////
    v_Visible = 1;

    if (a_isDigged == 0.0)
    {
        v_Visible = 0;
    }
    if (a_isIsolated == 0.0)
    {
        v_Visible = 0;
    }

    vec3 min_slice = vec3(u_Transform * vec4(u_min, 1.0));
    vec3 max_slice = vec3(u_Transform * vec4(u_max, 1.0));

    vec4 temp_dir = vec4(normalize(u_slice_direction), 0.0);
    if (u_slice_locked)
    {
        temp_dir = u_Transform * temp_dir;
    }

    vec3 slice_dir = temp_dir.xyz;

    vec3 slice_point = max_slice + u_slice_depth * (min_slice - max_slice);
    vec3 dir = slice_dir;
    vec3 center = vec3(u_Transform * vec4(a_Center, 1.0));
    float angle = dot(normalize(dir), normalize(center - slice_point));

    if (a_peel_depth < u_peel_depth || angle > 0)
    {
        v_Visible = 0;
    }
    ////////////////////////////////////////////////////////
    // Rounding
    ////////////////////////////////////////////////////////
    mat3 inverse_transform = mat3(transpose(inverse(u_Transform)));
    vec3 position = a_Pos;
    if (u_rounding)
    {
        float type = a_rounded_vertex_type;
        float r = u_rounding_size * u_average_cell_size * 0.3;
        // this vertex lies on the inner triangle
        if (type == ROUNDED_VERTEX_TYPE_FACE)
        {
            vec3 face_center = a_rounded_face_center_or_to_vertex.xyz;
            vec3 dir = face_center - position;
            float len = length(dir);
            float dist = r;
            position += normalize(dir) * dist;
        }
        // this vertex lies on an edge
        else if (type == ROUNDED_VERTEX_TYPE_EDGE)
        {
            float angle = a_rounded_face_center_or_to_vertex.w;
            vec3 to_vertex = a_rounded_face_center_or_to_vertex.xyz;
            vec3 dir = to_vertex - position;
            float dist = min(EDGE_FACTOR * r, length(dir) * 0.5);
            vec3 move_dir = normalize(a_Center - position);
            position += normalize(dir) * dist + move_dir * get_shrink_factor(angle, dist);
        }
        // this is a corner vertex
        else if (type == ROUNDED_VERTEX_TYPE_CORNER)
        {
            float angle = a_rounded_face_center_or_to_vertex.w;
            vec3 dir = a_rounded_face_center_or_to_vertex.xyz;
            float dist = CORNER_FACTOR * r;
            position += dir * get_shrink_factor(angle, dist);
        }
    }
    ////////////////////////////////////////////////////////

    mat4 light_space_mat = u_light_projection * u_light_view * u_light_transform;

    vec3 pos = a_Center + (position - a_Center) * u_cell_size;
    v_Pos = vec3(u_Transform * vec4(pos, 1.0));
    v_Normal = mat3(inverse_transform) * -a_Normal;
    v_LightSpacePos = light_space_mat * vec4(pos, 1.0);
    v_isTriangle = (a_isTriangle == 0.0) ? 0 : 1;

    v_Color = vec4(mix(u_objectColor.rgb, a_Color.rgb, a_Color.a), 1.0);

    if(a_isSelected > 0.0)
    {
        // Colorness is 0, if all rgb values are the same, and above 1 if they have a color difference
        float colorness = abs(v_Color.x - v_Color.y) + abs(v_Color.x - v_Color.z);

        vec3 inverse_color = vec3(1 - v_Color.x, 1 - v_Color.y , 1 - v_Color.z);

        vec3 selection_color = colorness * inverse_color + (1-colorness) * vec3(1,0,0);

        selection_color = normalize(selection_color);

        // Override inverse color with preset selection color
        selection_color = mix(selection_color, u_selection_color.xyz, u_selection_color.w);
        v_Color =vec4(selection_color, 1);

    }
}