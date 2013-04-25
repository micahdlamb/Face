#version 330 core

uniform sampler2D tex;

in vec2 coords;
out vec4 outColor;

void main(void){
	outColor = texture(tex,coords);
}