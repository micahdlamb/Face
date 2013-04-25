#pragma once

void pushEye(mat4& projection, mat4& view);
void popEye();

struct Light {
	vec3 color;

	Framebuffer fb;
	Texture depth;

	mat4 projection;
	mat4 view;

	Shader shadowDrawer;
	DepthBlurrer blurrer;

	Light(vec3 color, GLuint dim, mat4 projection, mat4 view):
		color(color)
		,depth(dim,dim,GL_RGB32F)
		,projection(projection)
		,view(view){
			blurrer(&depth);
			
			fb(&depth);
	}

	mat4 eye(){
		return projection * view;
	}

	vec3 pos(){
		return vec3(inverse(view)[3]);
	}

	void bind(){
		Viewport::push(0,0,depth.width, depth.height);
		fb.bind();
		glClearColor(1,1,1,1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pushEye(projection, view);
	}

	void unbind(){
		popEye();
		fb.unbind();
		Viewport::pop();
		blurrer.blur();
	}
};


//experimental (tries to dynamically update the lights eye to look at the view Frustum... works but kind of buggy)
/*
struct DirLight {
	vec3 dir;
	vec3 color;

	Framebuffer fb;
	Texture depth;

	mat4 projection;
	mat4 view;

	Shader shadowDrawer;
	DepthBlurrer blurrer;

	DirLight(vec3 dir, vec3 color, GLuint dim):
		color(color)
		,depth(NULL,dim,dim,GL_RGB32F)
		,dir(dir){
			blurrer(&depth);
			
			fb(&depth);
	}

	mat4 eye(){
		auto& vf = Scene::current->viewFrustum();
		auto pos = vf.center + -dir*1000.f;
		Scene::current->globals.lights[0].pos = pos;
		view = lookAt(pos, vf.center,vec3(0,1,0));
		//view = lookAt(pos, vec3(0,0,0),vec3(0,1,0));
		vec3 corners[8];
		for (int i=0; i < 8; i++)
			corners[i] = vec3(view * vec4(vf.corners[i],1));
		BoundingAABB b(corners,8);
		projection = ortho(b.min.x,b.max.x,b.min.y,b.max.y,-b.max.z,-b.min.z);
		//projection = perspective(60.f,1.f,100.f,1000.f);
		return projection * view;
	}

	void bind(){
		Viewport::push(0,0,depth.width, depth.height);
		fb.bind();
		glClearColor(1,1,1,1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void unbind(){
		fb.unbind();
		Viewport::pop();
		blurrer.blur();
	}
};
*/