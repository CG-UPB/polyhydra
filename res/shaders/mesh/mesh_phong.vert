#version 330 core

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Center;
layout (location = 3) in float a_peel_depth;
layout (location = 4) in float a_isDigged;
layout (location = 5) in vec4 a_Color;
layout (location = 6) in float a_isIsolated;
layout (location = 7) in float a_isTriangle;
layout (location = 8) in float a_rounded_vertex_type;
layout (location = 9) in vec3 a_rounded_face_center_or_to_vertex;

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
const float CORNER_FACTOR = sqrt(6.0) / 5.0;

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
    vec3 center =  vec3(u_Transform * vec4(a_Center, 1.0));
    float angle = dot(normalize(dir), normalize(center - slice_point));

    if (a_peel_depth < u_peel_depth || angle > 0)
    {
        v_Visible = 0;
    }
    ////////////////////////////////////////////////////////

    vec3 position = a_Pos;
    if (u_rounding)
    {
        float r = u_rounding_size;
        // this vertex lies on the inner triangle
        if (a_rounded_vertex_type == ROUNDED_VERTEX_TYPE_FACE)
        {
            vec3 face_center = a_rounded_face_center_or_to_vertex;
            vec3 dir = face_center - position;
            float len = length(dir);
            position += normalize(dir) * min(r, len);
        }
        // this vertex lies on an edge
        else if (a_rounded_vertex_type == ROUNDED_VERTEX_TYPE_EDGE)
        {
            vec3 to_vertex = a_rounded_face_center_or_to_vertex;
            vec3 dir = to_vertex - position;
            float len = length(dir);
            position += normalize(dir) * min(EDGE_FACTOR * r, len * 0.5) + a_Normal * (r - EDGE_FACTOR * r);
        }
        // this is a corner vertex
        else if (a_rounded_vertex_type == ROUNDED_VERTEX_TYPE_CORNER)
        {
            position += a_Normal * min(r - CORNER_FACTOR * r, length(position - a_Center));
        }
    }

    mat4 light_space_mat = u_light_projection * u_light_view * u_light_transform;

    vec3 pos = a_Center + (position - a_Center) * u_cell_size;
    v_Pos = vec3(u_Transform * vec4(pos, 1.0));
    v_Normal = mat3(transpose(inverse(u_Transform))) * -a_Normal;
    v_Color = a_Color;
    v_LightSpacePos = light_space_mat * vec4(pos, 1.0);
    v_isTriangle = (a_isTriangle == 0.0) ? 0 : 1;
}