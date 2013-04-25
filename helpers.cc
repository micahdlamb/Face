#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "includes.h"

using namespace std;

//no substr for char*? ... convert to string
string getFileNameWithoutExtension(string path){
	uint b = path.find_last_of( '/' )
		,e = path.find_last_of('.');
	b = b == string::npos ? 0 : (b+1);
	path = path.substr(b, (e==string::npos?path.size():e)-b);
	return path;
}

string getFileExtension(string path){
	return path.substr(path.find_last_of(".") + 1);
}

vec3 rainbowColors(float frequency){
	static int counter = 0;
	counter++;
	float red   = sin(frequency*counter + 0) * 127 + 128;
	float green = sin(frequency*counter + 2) * 127 + 128;
	float blue  = sin(frequency*counter + 4) * 127 + 128;
	return vec3(red/255,green/255,blue/255);
}

bool solveQuadratic(float &s1, float &s2, float a, float b, float c){
	float e = b*b - (a*c)*4;
	if (e < 0)
		return false;//imaginary solution
		
	float discriminant = sqrt(e);
	s1 = (-b - discriminant)/(2*a);
	s2 = (-b + discriminant)/(2*a);
	return true;
}

vec2 ndCoord(vec2 pt,vec2 dim){
	vec2 d = pt / dim;
	d.y = 1 - d.y;
	return d*2.0f - vec2(1);
}


void printDevILErrors(){
	ILenum err;
	while ((err = ilGetError()) != IL_NO_ERROR)
		msg("devIL error", iluErrorString(err));
}

string loadFile(string fname){
	ifstream fin(fname);
	if (!fin.is_open())
		error ("unable to open file: " + fname);
	stringstream ss;
	ss << fin.rdbuf();
	return ss.str();
}

void printGLErrors(const char* e){
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR){
		switch (error){
		case GL_INVALID_ENUM: msg(e,"GL_INVALID_ENUM"); break;
		case GL_INVALID_VALUE: msg(e,"GL_INVALID_VALUE"); break;
		case GL_INVALID_OPERATION: msg(e,"GL_INVALID_OPERATION"); break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: msg(e,"GL_INVALID_FRAMEBUFFER_OPERATION"); break;
		case GL_OUT_OF_MEMORY: msg(e,"GL_OUT_OF_MEMORY"); break;
		default: msg(e,"???");
		}
		Sleep(500);
	}
}


int ceilPow2(int val){
	int r = 2;
	for( ; r < val; r*=2 ) {}

	return r;
}

int factorial(int x) {
  return (x == 1 ? x : x * factorial(x - 1));
}

int n_choose_k(int n, int k){
	return factorial(n) / (factorial(k)*factorial(n-k));
}