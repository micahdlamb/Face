#version 330 core
in vec2 coords;

uniform vec2 coordScale;
uniform sampler2D tex;

out vec4 outColor;

vec2 gaussFilter[] = vec2[](
	vec2(-3.0,	0.015625),
	vec2(-2.0,	0.09375),
	vec2(-1.0,	0.234375),
	vec2(0.0,	0.3125),
	vec2(1.0,	0.234375),
	vec2(2.0,	0.09375),
	vec2(3.0,	0.015625)
);

void main()
{
	vec4 color = vec4(0.0);
	//this basically samples 3 pixels on either side of the current pixel and weights them according to the gaussFilter values
	//we will have to make 6 copies of this shader with different gaussFilter values.  We might also need to sample more than just the 3 closest pixels idk
	for( int i = 0; i < 7; i++ )
		color += texture2D( tex, vec2( coords.x + gaussFilter[i].x * coordScale.x, coords.y + gaussFilter[i].x * coordScale.y ) ) * gaussFilter[i].y;

	outColor = color;
}