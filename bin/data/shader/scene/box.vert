#version 400
uniform mat4 modelViewMatrix; // oF Default
uniform mat4 modelViewProjectionMatrix; // oF Default
in vec4 position; // oF Default
in vec4 normal; // oF Default
in vec4 color; // oF Default
in vec2 texcoord; // oF Default

uniform float lds;
uniform sampler2DRect col;
uniform sampler2DRect pos;
uniform sampler2DRect unit;

uniform int num;
uniform int barCount;
uniform float size;

out vec2 vUv;
out vec3 vC;
out vec3 vN;
out vec4 vP;
out float vD;
flat out float vAt;
flat out float vAc;

void main(){
    vec2 uv = vec2(gl_InstanceID+0.5, 0.5);
    vec3 c = texture(col, uv).rgb;
    vec3 p = texture(pos, uv).rgb;
    vec4 u = texture(unit, uv).rgba;

    float s = mix(0.5, 1.0, u.y);
    float zt = u.z * 16. * 0.5;
    float zs = u.z * 16. - (1. - s);
    vec3 ss = vec3(s, s, zs);

    vec3 pos = (p * 64. - vec3(8,8,barCount*16/2) + vec3(0, 0, zt)) * size;
    pos = pos + ss * position.xyz;

    vUv = texcoord;
    vP = modelViewMatrix * vec4(pos, 1.);
    vN = normal.xyz;
    vD = - vP.z * lds;
    vC = c;
    gl_Position = modelViewProjectionMatrix * vec4(pos, 1.);

    vAt = u.w;
    vAc = u.x;
}
