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
	float time;
	float deltaTime;
	vec2 screenDim;
};

layout(std140) uniform Object {
	mat4 worldTransform;
	mat3 normalTransform;
};

uniform sampler2D normals;
uniform sampler2D colors;
uniform sampler2D irradiance;
uniform sampler2D gauss0;
uniform sampler2D gauss1;
uniform sampler2D gauss2;
uniform sampler2D gauss3;
uniform sampler2D gauss4;
uniform sampler2D gauss5;
uniform sampler2D specular;
uniform sampler2D beckman;
uniform sampler3D perlin;
uniform sampler2D perturb;
uniform samplerCube reflections;

uniform float roughness = .3;
uniform float noiseFreq = .05;
uniform float noiseAmp = .08;
uniform int reflectionsOn;
uniform int phongOn;

in vec3 localPos;
in vec3 worldPos;
in vec2 uv;
//in vec2 uv0;
//in vec2 uv1;

out vec4 outColor;

float fresnelReflectance( vec3 H, vec3 V, float F0 ){
	float base = 1.0 - dot( V, H );
	float exponential = pow( base, 5.0 );
	return exponential + F0 * ( 1.0 - exponential );
}

float KS_Skin_Specular( vec3 N, // Bumped surface normal
	vec3 L, // Points to light
	vec3 V, // Points to eye
	float m,  // Roughness
	float rho_s) // Specular brightness
{
	float result = 0.0;
	float ndotl = dot( N, L );
	
	if( ndotl > 0.0 ){
		vec3 h = L + V; // Unnormalized half-way vector
		vec3 H = normalize( h );
		float ndoth = dot( N, H );
		float PH = pow( 2.0 * texture(beckman,vec2(ndoth,m)).r, 10.0 );
		float F = fresnelReflectance( H, V, 0.028 );
		float frSpec = max( PH * F / dot( h, h ), 0 );
		result = ndotl * rho_s * frSpec; // BRDF * dot(N,L) * rho_s
	}
	return result;
}

void main(void)
{
	/*
	vec2 uv = vec2(
		( fwidth( uv0.x ) < fwidth( uv1.x )-0.001 )? uv0.x : uv1.x
		,( fwidth( uv0.y ) < fwidth( uv1.y )-0.001 )? uv0.y : uv1.y
	);
	*/

	vec3 norm = normalTransform * (texture(normals, uv).rgb*2-vec3(1));

	/*
	norm += vec3(
		texture(perlin, localPos.xyz*noiseFreq + vec3(.5)).r
		,texture(perlin, localPos.zxy*noiseFreq + vec3(.5)).r
		,texture(perlin, localPos.yzx*noiseFreq + vec3(.5)).r
	)*noiseAmp;
	norm = normalize(norm);
	*/

	//i'm assuming the blue in the image refers to perturb along normal
	vec3 nz = norm;
	vec3 nx = cross(norm, vec3(0,1,0));
	vec3 ny = cross(norm, nx);
	vec3 n = texture(perturb, uv * noiseFreq).rgb * 2 - vec3(1);
	n = n.x * nx + n.y * ny + nz * n.z;
	norm += n * noiseAmp;
	norm = normalize(norm);

	if (!gl_FrontFacing) norm *= -1;

	//non blurred irradiance
	vec3 nondiffused = texture(irradiance, uv).rgb;
	//blurred irradiance
	vec3 diffused =
		texture(gauss0, uv).rgb * vec3(.22, .437, .635)
		+ texture(gauss1, uv).rgb * vec3(.101, .355, .365)
		+ texture(gauss2, uv).rgb * vec3(.119, .208, 0)
		+ texture(gauss3, uv).rgb * vec3(.114, 0, 0)
		+ texture(gauss4, uv).rgb * vec3(.364, 0, 0)
		+ texture(gauss5, uv).rgb * vec3(.08, 0, 0);

	//Color the surface
	float amb = .1;
	float shininess = 30;
	vec3 diffuseColor = texture(colors, uv).rgb;
	vec3 lightColor = lights[0].color;
	vec3 lightPos = lights[0].pos;
	vec3 lightDir = normalize(lightPos - worldPos);
	vec3 eyeDir = normalize(eyePos - worldPos);
	vec3 lightReflectDir = reflect(-lightDir, norm);
	vec3 eyeReflectDir = reflect(-eyeDir, norm);

	//outColor = texture(beckman,worldPos.xy);
	//return;

	

	//outColor = vec4(spec,spec,spec,1);
	//return;


	//ambient
	vec3 color = amb * mix(diffuseColor, textureLod(reflections, eyeReflectDir, 5).rgb, .2);
	
	//diffuse
	color += phongOn==1 ? nondiffused : diffused;
	
	//specular
	vec4 s = texture(specular, uv);//not sure what all the components represent
	//float m = s.w * .09 + .23;
	//float m = .3;//from http://developer.download.nvidia.com/presentations/2007/gdc/Advanced_Skin.pdf
	float rho_s = s.x * .16 + .18;//copied from advanced skin - final.cg, line 203
	color += lightColor * KS_Skin_Specular(norm, lightDir, eyeDir, roughness, rho_s);

	//outColor = vec4(vec3(KS_Skin_Specular(norm, lightDir, eyeDir, roughness, rho_s)), 1);
	//return;

	if (reflectionsOn == 1)
		color = mix(color, textureLod(reflections, eyeReflectDir, 5).rgb, .8);

	outColor = vec4(color,1);
}
