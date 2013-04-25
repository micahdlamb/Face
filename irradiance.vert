#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;

struct Light {
	mat4 eye;
	vec3 pos;
	vec3 color;
	vec4 pad[2];
};

layout(std140) uniform Global {
	mat4 eye;
	vec3 eyePos;
	int numLights;
	Light lights[6];
};

layout(std140) uniform Object {
	mat4 worldTransform;
	mat3 normalTransform;
};

out vec3 localPos;
out vec3 worldPos;
out vec2 uv;
out vec3 shadowCoords[6];

void main(void)
{
	uv = texCoord;
	localPos = pos;
	worldPos = vec3(worldTransform * vec4(pos,1));

	//shadow shit
	vec4 sc = lights[0].eye * vec4(worldPos,1);
	shadowCoords[0] = ((sc / sc.w) * .5 + .5).xyz;

	gl_Position = vec4(texCoord * 2 - vec2(1), 0, 1);
}