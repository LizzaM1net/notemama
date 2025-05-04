#version 450

layout(location = 1) in vec2 v_uv;
layout(location = 0) out vec4 fragColor;



void main() {

    vec2 buf_uv = v_uv;
    buf_uv -= 0.5;

    float d = length(buf_uv) - 0.2;
    vec3 col = vec3(step(0., -d));

    float glow = 0.01/d;

    glow = clamp(glow, 0., 1.);
    col += glow * 6.;
    col *= vec3(0.13, 0.12, 0.4);

    fragColor = vec4(col, 1.0);
}
