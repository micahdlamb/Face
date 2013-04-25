#version 330

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

in vec3 texCoord;
in vec3 worldPos;

out vec4 fragColor;
uniform samplerCube cubemap;

void main (void) {
	vec3 color = texture(cubemap, texCoord).rgb;
	
	//experimental: show light
	vec3 lightPos = lights[0].pos;
	vec3 lightColor = lights[0].color;
	float dist = length(lightPos - eyePos);
	float intensity = pow(max(0.0, dot(normalize(lightPos - eyePos), normalize(worldPos - eyePos))),dist*dist);
	if (intensity > .8) intensity = 1;
	if (intensity > .01)
		color = mix(color, lightColor, intensity);

	fragColor = vec4(color,1);
}