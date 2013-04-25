#version 330 core

layout(location = 0) in vec2 position;
layout(location = 2) in vec2 texCoords;

out vec2 uv;
void main(void)
{
	gl_Position = vec4(position, 0, 1.0);
	uv = texCoords;
}