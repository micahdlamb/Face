#pragma once

struct Scene {
	static Scene* current;
	struct GlobalUniforms : public UniformBlock {
		struct Light {
			mat4 eye;
			vec3 pos;
			float pad1;
			vec3 color;
			float pad2;
			vec4 pad3[2];//required in shader cuz nvidea and ati couldn't get their shit strait
		};
		
		mat4 eye;
		vec3 eyePos;
		float pad1;
		Light lights[6];
		float time;
		float deltaTime;
		vec2 screenDim;

		void operator()(){
			UniformBlock::operator()(sizeof(GlobalUniforms)-sizeof(UniformBlock),0);
		}

		//push everything, write individual setters eventually
		void sync(){
			set(&eye);
		}

		void syncEye(mat4 m, vec3 pos){
			eye = m;
			eyePos = pos;
			set(&eye,0,sizeof(eye)+sizeof(eyePos));
		}

	} globals;

	Viewer* viewer;

	stack<mat4> projections, views;
	stack<Frustum> viewFrustums;

	vector<Light*> lights;

	void operator()(Viewer* viewer){
		this->viewer = viewer;
		projections.push(mat4(1));
		viewFrustums.push(mat4(1));
		globals();
		current = this;
	}

	//call once at beginning of frame
	void frame(){
		current = this;
		globals.eye = eye();
		globals.eyePos = eyePos();
		globals.time = Clock::time;
		globals.deltaTime = Clock::delta;
		//globals.screenDim set in resize

		for (U i=0; i < lights.size(); i++){
			Scene::globals.lights[i].color = lights[i]->color;
			Scene::globals.lights[i].pos = lights[i]->pos();
			Scene::globals.lights[i].eye = lights[i]->eye();
		}

		globals.sync();//upload everything
		viewFrustums.top() = Frustum(eye());
	}

	void resize(mat4 p, int w, int h){
		projections.top() = p;
		viewFrustums.top() = Frustum(eye());
		globals.screenDim = vec2(w,h);
	}

	virtual void draw()=0;

	mat4 view(){
		if (views.size())
			return views.top();
		else
			return viewer->matrix();
	}

	mat4 eye(){
		return projections.top() * view();
	}

	vec3 eyePos(){
		return vec3(inverse(view())[3]);
	}

	void syncEye(){
		globals.syncEye(eye(),eyePos());
	}

	void setView(mat4 view){
		views.top() = view;
		viewFrustums.top() = Frustum(eye());
		syncEye();
	}

	void pushEye(mat4 projection, mat4 view){
		projections.push(projection);
		views.push(view);
		syncEye();
		viewFrustums.push(eye());
	}

	void popEye(){
		projections.pop();
		views.pop();
		syncEye();
		viewFrustums.pop();
	}

	void pushProjection(mat4 m){
		projections.push(m);
		syncEye();
		viewFrustums.push(eye());
	}
	void popProjection(){
		projections.pop();
		syncEye();
		viewFrustums.pop();
	}

	void pushView(mat4 m){
		views.push(m);
		syncEye();
		viewFrustums.push(eye());
	}

	void popView(){
		views.pop();
		syncEye();
		viewFrustums.pop();
	}

	void addLight(Light* light){
		lights.push_back(light);
	}

	Frustum& viewFrustum(){
		if (viewFrustums.size() == 0)
			cout <<"tits";
		return viewFrustums.top();
	}
};

inline void pushEye(mat4& projection, mat4& view){
	Scene::current->pushEye(projection, view);
}

inline void popEye(){
	Scene::current->popEye();
}

/*
struct GlobalUniforms : public UniformBlock {
	struct Light {
		mat4 eye;
		vec3 pos;
		float pad1;
		vec3 color;
		float pad2;
		vec4 pad3[2];//required in shader cuz nvidea and ati couldn't get their shit strait
	};
		
	mat4 eye;
	vec3 eyePos;
	float pad1;
	Light lights[6];

	
	void operator()(){
		UniformBlock::operator()(0, sizeof(GlobalUniforms)-sizeof(UniformBlock));
	}

	//push everything, write individual setters eventually
	void push(){
		set(0, sizeof(GlobalUniforms)-sizeof(UniformBlock), &eye);
	}
};

struct Scene {
	struct GlobalUniforms {
		struct Light {
			mat4 eye;
			vec3 pos;
			float pad1;
			vec3 color;
			float pad2;
			vec4 pad3[2];//required in shader cuz nvidea and ati couldn't get their shit strait
		};
		
		mat4 eye;
		vec3 eyePos;
		float pad1;
		Light lights[6];
	};
	
	struct Stack : public stack<mat4>{
		Scene* s;//meh unneeded ptr but not sure how else to keep syntax clean
		Stack(Scene* s):stack(),s(s){}
		void push(const mat4& m){
			stack::push(m);
			gpu();
		}
		void pop(){
			stack::pop();
			gpu();
		}
		void operator=(mat4& m){
			top() = m;
			gpu();
		}
		void gpu(){
			mat4 m = s->projection.top() * s->lookAt.top();
			s->global.set(offsetof(GlobalUniforms,eye), sizeof(mat4),&m);
		}


	};
	
	Scene():projection(this),lookAt(this){}

	Viewer viewer;
	Stack projection;
	Stack lookAt;


	UniformBlock global;


}
*/