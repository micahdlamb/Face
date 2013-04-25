#version 330
layout(location = 0) in vec3 vertex;

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

out vec3 worldPos;
out vec3 texCoord;

void main() {
	worldPos = vertex * 1000000;//bigger than any scene
	gl_Position = eye * vec4(worldPos, 1);
	gl_Position.z = gl_Position.w;//never culled
	texCoord = vertex;
}