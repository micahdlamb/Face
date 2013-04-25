#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;

layout(std140) uniform Object {
	mat4 worldTransform;
	mat3 normalTransform;
};

out vec3 localPos;
out vec3 worldPos;

void main(void)
{
	localPos = pos;
	worldPos = vec3(worldTransform * vec4(pos,1));
	gl_Position = vec4(texCoord * 2 - vec2(1), 0, 1);
}