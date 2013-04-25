#pragma once

struct Shader {
	#define SHADER_PARAMS const char* vert, const char* frag=NULL, const char* geo=NULL, const char* tcs=NULL, const char* tes=NULL
	#define SHADER_CALL vert,frag,geo,tcs,tes

	GLuint gid;

	//allow delayed construction if needed
	Shader(){}
	Shader(SHADER_PARAMS){ operator()(SHADER_CALL); }

	void operator()(SHADER_PARAMS, bool link=true){
		gid = glCreateProgram();
		attachModules(SHADER_CALL);
		if (link)
			this->link();
	}
	
	void attachModules(SHADER_PARAMS){
		glAttachShader(gid, getModule(vert,GL_VERTEX_SHADER));
		if (frag) glAttachShader(gid, getModule(frag,GL_FRAGMENT_SHADER));
		if (geo) glAttachShader(gid, getModule(geo,GL_GEOMETRY_SHADER));
		if (tcs) glAttachShader(gid, getModule(tcs,GL_TESS_CONTROL_SHADER));
		if (tes) glAttachShader(gid, getModule(tes,GL_TESS_EVALUATION_SHADER));
	}

	/*
	void link(){
		GLint link_ok = GL_FALSE;
		glLinkProgram(gid);
		glGetProgramiv(gid, GL_LINK_STATUS, &link_ok);
		if (!link_ok)
			compileFail("link: ", gid);

		//Automatically make a few shader blocks available to any shader, this can be specified in the shader in latest glsl
		addUniformBlock("Global",0);
		enable();
	}
	*/

	void link(){
		GLint success = GL_FALSE;
		glLinkProgram(gid);
		glGetProgramiv(gid, GL_LINK_STATUS, &success);
		if (!success){
			GLint len = 0;
			glGetProgramiv(gid, GL_INFO_LOG_LENGTH, &len);
			string log;
			log.resize(len);
			glGetProgramInfoLog(gid, len, NULL, (GLchar*)log.c_str());
			error("link fail: " + log);
		}

		//Automatically make a few shader blocks available to any shader, this can be specified in the shader in latest glsl
		addUniformBlock("Global",0);
		enable();
	}

	void enable(){
		static Shader* enabled=NULL;//won't work for multiple GL contexts
		if (this != enabled)
			glUseProgram(gid);
		enabled = this;
	}

	void addUniformBlock(const char* name, GLuint binding){
		GLuint global =  glGetUniformBlockIndex(gid, name);
		if (global != GL_INVALID_INDEX)
			glUniformBlockBinding(gid, global, binding);
		//else
			//error(string(name) + " can't be bound to " + itos(binding));
	}

	~Shader(){
		glDeleteProgram(gid);
		//printGLErrors("/~Shader");
	}

	private:
	/*
	GLuint getModule(const char* file, GLenum type){
		static map<string,GLuint> cache;
		auto i = cache.find(file);
		if (i == cache.end()){
			GLint compiled;
			string src = loadFile(file);
			GLuint sid = glCreateShader(type);
			const char* cstr = src.c_str();//any way to avoid this?
			const GLint size = src.size();
			glShaderSource(sid, 1, &cstr, &size);
			glCompileShader(sid);
			glGetShaderiv(sid, GL_COMPILE_STATUS, &compiled);
			if (!compiled)
				compileFail(file, sid);

			cache[file] = sid;
			return sid;
		} else 
			return i->second;
	}
	*/

	GLuint getModule(const char* file, GLenum type){
		static map<string,GLuint> cache;
		auto i = cache.find(file);
		if (i == cache.end()){
			auto sid = compile(file, type);
			cache[file] = sid;
			return sid;
		} else
			return i->second;
	}

	GLuint compile(const char* file, GLenum type){
		string src = loadFile(file);
		GLuint sid = glCreateShader(type);
		const char* cstr = src.c_str();//any way to avoid this?
		const GLint size = src.size();
		glShaderSource(sid, 1, &cstr, &size);
		
		GLint len = 0, success;
		glCompileShader(sid);
		glGetShaderiv(sid, GL_COMPILE_STATUS, &success);
		glGetShaderiv(sid, GL_INFO_LOG_LENGTH, &len);
		string log;
		log.resize(len);
		glGetShaderInfoLog(sid, len, NULL, (GLchar*)log.c_str());
		if (success){
			if (len > 1)
				msg(string("warnings in ")+file,log);
		} else
			error(string(file) + ": " + log);
		return sid;
	}

	/*
	void compileFail(string msg, GLuint o){
		GLint len = 0;
		if (glIsShader(o))
			glGetShaderiv(o, GL_INFO_LOG_LENGTH, &len);
		else if (glIsProgram(o))
			glGetProgramiv(o, GL_INFO_LOG_LENGTH, &len);
		else
			error("compileFail: what is this?");

		string log;
		log.resize(len);
		if (glIsShader(o))
			glGetShaderInfoLog(o, len, NULL, (GLchar*)log.c_str());
		else if (glIsProgram(o))
			glGetProgramInfoLog(o, len, NULL, (GLchar*)log.c_str());
		error(msg + ": " + log);
	}
	*/
	//prevent copies
	Shader(const Shader& s);
	Shader& operator=(Shader s);
};