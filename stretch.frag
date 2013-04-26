#version 330 core

in vec3 localPos;
in vec3 worldPos;
out vec4 outColor;

uniform vec2 dim;

void main(void)
{
	vec3 du = dFdx( worldPos );
	vec3 dv = dFdy( worldPos );
	float scale = .2;//make output fit in range 0->1, must unscale this when using
	vec2 stretch = vec2(length( du ), length( dv )) * dim * scale;
	//outputs 1 / dist in world space that each pixel covers
	outColor = vec4(1 / stretch, 0, 1); // A two-component texture
}