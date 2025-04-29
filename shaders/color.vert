#version 440

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

layout(location = 0) out vec4 v_color;

layout(std140, binding = 0) uniform buf {
    mat4 mvp;
};

void main()
{
    v_color = color;
    gl_Position = mvp * position;
}
