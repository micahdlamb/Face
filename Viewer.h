#pragma once

struct Viewer {
	vec3 pos,forward,world_up;
	Viewer():pos(0,0,0),forward(0,0,-1),world_up(0,1,0){}
	
	void operator()(vec3 pos=vec3(0,0,0), vec3 forward=vec3(0,0,-1), vec3 world_up=vec3(0,1,0)){
		this->pos = pos; this->world_up = world_up; this->forward = forward;
	}

	mat4 matrix(){
		return lookAt(pos, pos + forward, world_up);
	}

	vec3 right(){
		return normalize(cross(forward,world_up));
	}

	vec3 up(){
		return normalize(cross(right(),forward));
	}

	void origin(){
		pos = vec3(0);
		forward = vec3(0,0,-1);
	}

	void turn(float x, float y){
		forward = mat3(rotate(mat4(1),-x,world_up)) * forward;
		forward = mat3(rotate(mat4(1),y,right())) * forward;
	}
};