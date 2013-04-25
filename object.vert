#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;

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
	mat4 normalTransform;
};

out vec3 localPos;
out vec3 worldPos;
out vec3 normal;

out vec3 shadowCoords[6];

void main(void)
{
	localPos = pos;
	worldPos = vec3(worldTransform * vec4(pos,1));
	vec4 sc = lights[0].eye * vec4(worldPos,1);
	sc = sc / sc.w;
	sc = sc * .5 + .5;
	shadowCoords[0] = sc.xyz;
	normal = vec3(normalTransform * vec4(norm,0));
	gl_Position = eye * vec4(worldPos, 1);
}