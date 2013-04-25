#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoords;

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
//out vec2 uv0;
//out vec2 uv1;

void main(void)
{
	uv = texCoords;
	//remove texture seam (doesn't seem to work on this model)
	//uv0 = fract(uv);
	//uv1 = fract(uv + vec2(.5)) - vec2(.5);

	localPos = pos;
	worldPos = vec3(worldTransform * vec4(pos,1));
	gl_Position = eye * vec4(worldPos, 1);
}