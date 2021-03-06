#version 330 core
layout (location = 0) in vec3 vertex_position; 
layout (location = 1) in vec2 uv;

out vec2 TexCoord;

void main()
{
    TexCoord = uv;
    gl_Position = vec4(vertex_position, 1.0);
}