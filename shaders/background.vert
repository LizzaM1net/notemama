#version 450

layout(location = 0) in vec2 a_position;
layout(location = 1) out vec2 v_uv;

layout(std140, binding = 0) uniform buf {
    mat4 mvp;
};


void main() {
    v_uv = a_position * 0.5 + 0.5; // чтобы перевести из [-1, 1] в [0, 1]
    gl_Position = mvp*  vec4(a_position, 0.0, 1.0);
}

