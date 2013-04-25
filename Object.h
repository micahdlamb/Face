#pragma once

struct Object : public Shader {
	struct Shared {
		mat4 worldTransform;
		mat4 normalTransform;//use mat3 in the shader
		mat4 inverseTransform;
	};
	

	GlobalUniform<Shared> shared;
	VAO* vao;

	IBoundingVolume* boundingVolume;

	Shader shadow;
	bool castShadows;

	Object(){}
	Object(mat4 transform,SHADER_PARAMS){operator()(transform,SHADER_CALL);}

	void operator()(mat4 transform, SHADER_PARAMS){
		Shader::operator()(SHADER_CALL);
		vao = NULL;
		boundingVolume = NULL;
		castShadows = false;
		shared(1);
		addShader(this);
		setWorldTransform(transform);
	}

	virtual void enableShadowCast(const char* vert="shadow.vert", const char* frag="shadow.frag", const char* geo=NULL, const char* tcs=NULL, const char* tes=NULL){
		castShadows = true;
		shadow(SHADER_CALL);
		addShader(&shadow);
	}

	//call before any draw
	void ready(){
		shared.enable();
	}

	virtual void draw(){
		if (cull()) return;
		Shader::enable();
		ready();
		vao->draw();
	}

	virtual void shadowDraw(){
		if (!castShadows || cull()) return;
		shadow.enable();
		ready();
		vao->draw();
	}

	void addShader(Shader* shader){
		shared.addShader(shader,"Object");
	}

	void setVAO(VAO* vao){
		this->vao = vao;
		syncBoundingVolume();
	}

	void setWorldTransform(mat4 m){
		(*shared).worldTransform = m;
		(*shared).inverseTransform = inverse(m);
		(*shared).normalTransform = transpose((*shared).inverseTransform);
		shared.push();

		syncBoundingVolume();
	}

	mat4 getWorldTransform(){
		return (*shared).worldTransform;
	}

	mat4 getNormalTransform(){
		return (*shared).normalTransform;
	}

	mat4& getInverseTransform(){
		return (*shared).inverseTransform;
	}

	void changeWorldTransform(mat4 m){
		setWorldTransform(m * getWorldTransform());
	}

	bool cull(){
		return boundingVolume && !boundingVolume->inside(Scene::current->viewFrustum());
	}

	void syncBoundingVolume(){
		if (vao && vao->boundingVolume)
			vao->boundingVolume->transform(getWorldTransform(), boundingVolume);
		else
			boundingVolume = NULL;
	}
};
