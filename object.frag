#version 330 core

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

float chebyshevUpperBound(float distance, vec2 moments)
{
	// Surface is fully lit. as the current fragment is before the light occluder
	if (distance <= moments.x)
		return 1.0 ;

	// The fragment is either in shadow or penumbra. We now use chebyshev's upperBound to check
	// How likely this pixel is to be lit (p_max)
	float variance = moments.y - (moments.x*moments.x);
	variance = max(variance,0.00002);

	float d = distance - moments.x;
	float p_max = variance / (variance + d*d);

	return p_max;
}

uniform int selected;
uniform int asleep;
uniform vec3 color;

uniform sampler2D shadowMap;
in vec3 shadowCoords[6];

in vec3 localPos;
in vec3 worldPos;
in vec3 normal;
out vec4 outColor;

void main(void)
{
	//hardcoded light and color for now
	float amb = .25;
	float dif = .8;
	float spec = 1;
	float shininess = 30;
	vec3 norm = normalize(normal);
	if (!gl_FrontFacing) norm *= -1;

	//Color the surface
	vec3 diffuseColor = color;

	vec3 lightColor = lights[0].color;
	vec3 lightPos = lights[0].pos;
	vec3 lightDir = normalize(lightPos - worldPos);
	vec3 eyeDir = normalize(eyePos - worldPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	if (selected==1){
		/*
		float sillouette = pow(1 - max(0, dot(norm, eyeDir)), 1);
		if (sillouette > .6)
			diffuseColor = vec3(0,0,0);
		*/
		vec3 color1 = diffuseColor;
		vec3 color2 = vec3(1) - diffuseColor;
		float stripes = 10.f;
		diffuseColor = int((localPos.y+1000) * stripes) % 2 == 0 ? color1 : color2;
	}

	vec2 moments0 = texture(shadowMap, shadowCoords[0].xy).rg;
	float shadowedIntensity = pow(chebyshevUpperBound(shadowCoords[0].z, moments0),4);

	//ambient (give a little directional light)
	vec3 color = mix(diffuseColor, diffuseColor * max(0, dot(norm, vec3(0,1,0))), .3) * amb;
	//vec3 color = diffuseColor * amb;
	//diffuse
	float d = max(0.0, dot(norm, lightDir));
	color += shadowedIntensity * diffuseColor * dif * d;
	//specular
	if (d > .1)//experimental, get rid of light pixels on sillouette
		color += shadowedIntensity * lightColor * spec * pow(max(0.0, dot(reflectDir, eyeDir)), shininess);
	
	//experimental: show light
	/*
	if (length(lightPos - eyePos) < length(worldPos - eyePos)){
		float dist = length(lightPos - eyePos);
		float intensity = pow(max(0.0, dot(normalize(lightPos - eyePos), normalize(worldPos - eyePos))),dist*dist);
		if (intensity > .8) intensity = 1;
		if (intensity > .01)
			color = mix(color, lightColor, intensity);
	}
	*/
	outColor = vec4(color,asleep==1?.5:1);
}