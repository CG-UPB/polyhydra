#version 330 core

#define PI 3.14159265359

in vec2 v_uv;

uniform vec3 u_color;
uniform vec3 u_light_color;
uniform vec3 u_light_dir;
uniform vec3 u_view_dir;
uniform vec3 u_cam_pos;
uniform mat4 u_inv_projection;
uniform mat4 u_inv_view;

out vec4 FragColor;

float noise(float t)
{
    return 1.0;//texture(iChannel0,vec2(t,.0)/iChannelResolution[0].xy).x;
}
float noise(vec2 t)
{
    return 1.0;//texture(iChannel0,t/iChannelResolution[0].xy).x;
}

vec3 lensflare(vec2 uv, vec2 pos)
{
    vec2 main = uv-pos;
    vec2 uvd = uv*(length(uv));

    float ang = atan(main.x,main.y);
    float dist=length(main); dist = pow(dist,.1);
    float n = noise(vec2(ang*16.0,dist*32.0));

    float f0 = 1.0/(length(uv-pos)*16.0+1.0);

    f0 = f0 + f0*(sin(noise(sin(ang*2.+pos.x)*4.0 - cos(ang*3.+pos.y))*16.)*.1 + dist*.1 + .8);

    float f1 = max(0.01-pow(length(uv+1.2*pos),1.9),.0)*7.0;

    float f2 = max(1.0/(1.0+32.0*pow(length(uvd+0.8*pos),2.0)),.0)*00.25;
    float f22 = max(1.0/(1.0+32.0*pow(length(uvd+0.85*pos),2.0)),.0)*00.23;
    float f23 = max(1.0/(1.0+32.0*pow(length(uvd+0.9*pos),2.0)),.0)*00.21;

    vec2 uvx = mix(uv,uvd,-0.5);

    float f4 = max(0.01-pow(length(uvx+0.4*pos),2.4),.0)*6.0;
    float f42 = max(0.01-pow(length(uvx+0.45*pos),2.4),.0)*5.0;
    float f43 = max(0.01-pow(length(uvx+0.5*pos),2.4),.0)*3.0;

    uvx = mix(uv,uvd,-.4);

    float f5 = max(0.01-pow(length(uvx+0.2*pos),5.5),.0)*2.0;
    float f52 = max(0.01-pow(length(uvx+0.4*pos),5.5),.0)*2.0;
    float f53 = max(0.01-pow(length(uvx+0.6*pos),5.5),.0)*2.0;

    uvx = mix(uv,uvd,-0.5);

    float f6 = max(0.01-pow(length(uvx-0.3*pos),1.6),.0)*6.0;
    float f62 = max(0.01-pow(length(uvx-0.325*pos),1.6),.0)*3.0;
    float f63 = max(0.01-pow(length(uvx-0.35*pos),1.6),.0)*5.0;

    vec3 c = vec3(.0);

    c.r+=f2+f4+f5+f6; c.g+=f22+f42+f52+f62; c.b+=f23+f43+f53+f63;
    c = c*1.3 - vec3(length(uvd)*.05);
    c+=vec3(f0);

    return c;
}

vec3 cc(vec3 color, float factor, float factor2) // color modifier
{
    float w = color.x + color.y + color.z;
    return mix(color, vec3(w) * factor, w * factor2);
}

vec4 get_position(vec2 uv)
{
    vec4 point = u_inv_projection * vec4(uv.xy * 2.0 - 1.0, 1.0, 1.0);
    point = point / point.w;
    return u_inv_view * point;
}

float map(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{
    vec3 pos_ws = get_position(v_uv).xyz;
    vec3 frag_dir = normalize(pos_ws - u_cam_pos);
    vec3 color = mix(u_color, u_light_color, pow(abs(1.0 - frag_dir.y), 6.0));
    color = mix(color, vec3(1.0), pow(abs((1.0 - frag_dir.y) * 0.5), 6.0));
    FragColor = vec4(color.rgb, 1.0);
}