#version 330 core

out vec4 FragColor;

in vec3 v_Pos;
in vec3 v_Normal;

uniform vec3 u_lightPos;
uniform vec3 u_camPos;
uniform vec3 u_lightColor;
uniform vec3 u_objectColor;

void main()
{
        vec3 tangent = vec3(1.0,1.0,1.0);

        vec3 c1 = cross(v_Normal, vec3(0.0, 0.0, 1.0));
        vec3 c2 = cross(v_Normal, vec3(0.0, 1.0, 0.0));

        if (length(c1)>length(c2))
        {
            tangent = c1;
        }
        else
        {
            tangent = c2;
        }

        FragColor = vec4(normalize(tangent), 1.0);
}