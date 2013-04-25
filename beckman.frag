#version 330 core
in vec2 coords;
out vec4 outColor;

float PHBeckmann( float ndoth, float m )  {
	float alpha = acos( ndoth );
	float ta = tan( alpha );
	float val = 1.0/(m*m*pow(ndoth,4.0))*exp(-(ta*ta)/(m*m));
	return val;
}

void main(void){
	float val = 0.5 * pow( PHBeckmann( coords.x, coords.y ), 0.1 );
	outColor = vec4(val,0,0,1);
}