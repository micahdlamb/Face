#pragma once

struct Uniform {
	GLint location;
	Uniform(){}
	Uniform(const char* name, Shader* shader){
		operator()(name,shader);
	}

	void operator()(const char* name, Shader* shader){
		location = glGetUniformLocation(shader->gid, name);
		if (location == -1)
			msg("Could not bind uniform",name);
	}

	virtual void push()=0;
};


//Can't figure out how to do these with templates, so I'll just hardcode them as needed


struct Uniform1i : public Uniform {
	GLint value;

	void push(){
		if (location != -1){
			glUniform1iv(location, 1, &value);
		}
	}

	GLint& operator *(){ return value; }

	void operator()(const char* name, Shader* shader){
		Uniform::operator()(name, shader);
	}

	void operator()(const char* name, Shader* shader, GLint v){
		Uniform::operator()(name, shader);
		shader->enable();
		*this = v;
	}

	GLint & operator=(const GLint v){
		if (v != value){
			value = v;
			push();
		}
		return value;
	}
};

struct Uniform1f : public Uniform {
	GLfloat value;
	
	void push(){
		if (location != -1){
			glUniform1fv(location, 1, &value);
		}
	}

	GLfloat& operator *(){ return value; }

	void operator()(const char* name, Shader* shader){
		Uniform::operator()(name, shader);
	}

	void operator()(const char* name, Shader* shader, GLfloat v){
		Uniform::operator()(name, shader);
		shader->enable();
		*this = v;
	}

	GLfloat & operator=(const GLfloat v){
		value = v;
		push();
		return value;
	}
};

struct Uniform2f : public Uniform {
	vec2 value;
	void push(){
		if (location != -1){
			glUniform2fv(location, 1, value_ptr(value));
		}
	}

	vec2& operator *(){ return value; }

	void operator()(const char* name, Shader* shader){
		Uniform::operator()(name, shader);
	}

	void operator()(const char* name, Shader* shader, vec2 v){
		Uniform::operator()(name, shader);
		shader->enable();
		*this = v;
	}

	vec2 & operator=(const vec2 &v){
		value = v;
		push();
		return value;
	}
};

struct Uniform3f : public Uniform {
	vec3 value;
	void push(){
		if (location != -1){
			glUniform3fv(location, 1, value_ptr(value));
		}
	}
	
	vec3& operator *(){ return value; }

	void operator()(const char* name, Shader* shader){
		Uniform::operator()(name, shader);
	}

	void operator()(const char* name, Shader* shader, vec3 v){
		Uniform::operator()(name, shader);
		shader->enable();
		*this = v;
	}

	vec3 & operator=(const vec3 &v){
		value = v;
		push();
		return value;
	}
};

struct Uniform4f : public Uniform {
	vec4 value;
	void push(){
		if (location != -1){
			glUniform4fv(location, 1, value_ptr(value));
		}
	}
	
	vec4& operator *(){ return value; }

	void operator()(const char* name, Shader* shader){
		Uniform::operator()(name, shader);
	}

	void operator()(const char* name, Shader* shader, vec4 v){
		Uniform::operator()(name, shader);
		shader->enable();
		*this = v;
	}

	vec4 & operator=(const vec4 &v){
		value = v;
		push();
		return value;
	}
};

struct UniformMat4 : public Uniform {
	mat4 value;

	void push(){
		if (location != -1){
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(value));
		}
	}

	mat4& operator *(){ return value; }

	//matrices don't default to identity in shader
	void operator()(const char* name, Shader* shader, mat4 v=mat4(1)){
		Uniform::operator()(name, shader);
		shader->enable();
		*this = v;
	}

	mat4 & operator=(const mat4 &v){
		value = v;
		push();
		return value;
	}
};

struct UniformMat3 : public Uniform {
	mat3 value;
	void push(){
		if (location != -1){
			glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(value));
		}
	}

	void operator()(const char* name, Shader* shader, mat3 v=mat3(1)){
		Uniform::operator()(name, shader);
		shader->enable();
		*this = v;
	}

	mat3& operator *(){ return value; }
	
	mat3 & operator=(const mat3 &v){
		value = v;
		push();
		return value;
	}
};


struct UniformSampler : public Uniform {
	Texture* value;
	GLuint textureUnit;

	void set(Texture* value, GLuint textureUnit){
		this->value = value;
		this->textureUnit = textureUnit+1;//0 reserved for other texture operations
		if (this->textureUnit >= Texture::maxUnits())
			error("too many textures bound, you need to implement bufferbase functionality");
		push();
	}

	void push(){
		pushUniform();
		pushTexture();
	}

	void pushUniform(){
		printGLErrors("UniformSampler pushUniform");
		glUniform1i(location, textureUnit);
		printGLErrors("/UniformSampler pushUniform");
	}

	void pushTexture(){
		printGLErrors("UniformSampler pushTexture");
		value->bind(textureUnit);
		//Texture::activateUnit(textureUnit);
		//glBindTexture(value->target, value->gid);
		printGLErrors("/UniformSampler pushTexture");
	}

	//the max number of unique textures is currently limited by the number of texture units
	void operator()(const char* name, Shader* shader, Texture* value, bool shareUnit=true){
		assert(value && "UniformSampler logic currently requires it to be initialized with a Texture");
		Uniform::operator()(name,shader);
		static GLuint counter = 0;
		static map<Texture*,GLuint> units;
		GLuint u;
		if (shareUnit){
			auto i = units.find(value);
			if (i == units.end()){
				u = counter++;
				units[value] = u;
			} else
				u = i->second;
		} else
			u = counter++;
		shader->enable();
		set(value, u);
	}

	Texture* operator=(Texture* v){
		value = v;
		pushTexture();
		return value;
	}
};

//Uniform blocks allow you to pass in a structure to a shader.  You can also share them across multiple shaders using the addShader function 
struct UniformBlock {
	GLuint bid, binding, size;
	
	//all UniformBlocks in a single shader must have a unique bindings.
	//If you're sure your total number of UniformBlocks will stay under the max number of binding points (around 50 I think) then u can ignore the binding parameter.
	//Otherwise you must manually select binding and remember to call enable before any draw call
	void operator()(GLuint size, GLuint binding=-1){
		printGLErrors("UniformBlock");
		static GLuint counter = 2;//0 reserved for Global, 1 used for Object::shared
		this->binding = binding==-1 ? counter++ : binding;
		this->size = size;
		glGenBuffers(1, &bid);
		glBindBuffer(GL_UNIFORM_BUFFER, bid);
		glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STREAM_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferBase(GL_UNIFORM_BUFFER, this->binding, bid);
		printGLErrors("/UniformBlock");
	}

	//must enable before a draw if multiple distinct UniformBlocks using same binding point (like Object's shared).
	//This complication is required because their are only a finite number of binding points
	void enable(){
		glBindBufferBase(GL_UNIFORM_BUFFER, this->binding, bid);
	}

	//connect shader to this uniform block, unneccessary if binding already specified in shader
	void addShader(Shader* shader, const char* name){
		GLuint index = glGetUniformBlockIndex(shader->gid, name);
		glUniformBlockBinding(shader->gid, index, binding);
	}

	void set(GLvoid* value, GLuint start=0, GLuint size=-1){
		if (size==-1) size = this->size;
		glBindBuffer(GL_UNIFORM_BUFFER, bid);
		glBufferSubData(GL_UNIFORM_BUFFER, start, size, value);
	}
};


template <class T>
struct GlobalUniform : UniformBlock {
	T v;

	void operator()(T& t){
		UniformBlock::operator()(sizeof(T));
		*this = t;
	}
	void operator()(GLuint binding){
		UniformBlock::operator()(sizeof(T), binding);
	}
	T& operator*(){
		return v;
	}
	void push(){
		set(&v);
	}
	T& operator=(T& t){
		v = t;
		push();
		return v;
	}
};
