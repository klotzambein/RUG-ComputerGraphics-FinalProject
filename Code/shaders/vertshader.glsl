#version 330 core

// Specify the input locations of attributes
layout (location = 0) in vec2 pos_in;

// Specify the output of the vertex stage
out vec2 uv_in;

void main()
{
    gl_Position = vec4(pos_in * 2 - 1, 0, 1.0);

    uv_in = pos_in;
}
