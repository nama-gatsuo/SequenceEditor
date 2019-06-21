#version 400

uniform int isShadow;

in vec2 vUv;
in vec3 vC;
in vec3 vN;
in vec4 vP;
in float vD;
flat in float vAt;
flat in float vAc;

layout (location = 0) out vec4 outputColor0;
layout (location = 1) out vec4 outputColor1;
layout (location = 2) out vec4 outputColor2;

void main(void){

    if (isShadow == 0) {
        if (any(lessThan(vUv, vec2(0.1))) || any(greaterThan(vUv, vec2(0.9))) ) {
            outputColor0 = vec4(vC * 0.5, 1.);
            outputColor1 = vP;
            outputColor2 = vec4(vN, vD);
        } else {
            if (vAc > 0.5) {
                outputColor0 = vec4(vAt < 0.5 ? vC : vC * 6., 1.);
                outputColor1 = vP;
                outputColor2 = vec4(vN, vD);
            } else {
                discard;
            }
        }
    } else {
        if (any(lessThan(vUv, vec2(0.1))) || any(greaterThan(vUv, vec2(0.9))) ) {
            outputColor0.r = vD;
            outputColor0.a = 1.0;
        } else {
            if (vAc > 0.5) {
                outputColor0.r = vD;
                outputColor0.a = 1.0;
            } else {
                discard;
            }
        }

    }
}
