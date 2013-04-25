#version 330 core

in float z;
out vec4 outColor;

void main(void)
{
	float moment1 = z;
	float moment2 = z*z;

	float dx = dFdx(z);
	float dy = dFdy(z);
	//moment2 += .25 * (dx*dx + dy*dy);
	moment2 += .01 * (dx*dx + dy*dy);

	outColor = vec4( moment1,moment2, 0.0, 1 );
}