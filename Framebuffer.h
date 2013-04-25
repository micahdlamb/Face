#pragma once

struct Framebuffer {
	static const int MAXVPSIZE = 4096; //max render size (how to just use default framebuffer?)

	GLuint gid;
	vector<GLenum> buffers;
	Framebuffer(){}
	Framebuffer(bool init){if (init) operator()();}
	void operator()(){
		glGenFramebuffers(1, &gid);
		attachDefaultDepthBuffer();
	}

	void operator()(Texture* texture){
		operator()();
		bind();
		attach(texture);
		finalize();
		unbind();
	}

	~Framebuffer(){
		glDeleteFramebuffers(1, &gid);
	}

	static Framebuffer& instance(){
		static Framebuffer* f = new Framebuffer(true);
		return *f;
	}

	void attachDefaultDepthBuffer(){
		static GLuint depthBuffer = 0;
		if (!depthBuffer){
			glGenRenderbuffers(1, &depthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, MAXVPSIZE, MAXVPSIZE);
		}

		//bind depth buffer
		//glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);// might be needed
		bind();
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
		unbind();
	}
	
	void bind(GLenum binding=GL_FRAMEBUFFER){
		glBindFramebuffer(binding, gid);
	}

	void unbind(){
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}

	void attach(Texture* texture, GLenum attachment=GL_COLOR_ATTACHMENT0){
		glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture->gid, 0);
		
		if (attachment != GL_DEPTH_ATTACHMENT)
			buffers.push_back(attachment);
	}

	void attachCube(Texture* texture, GLenum face){
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, face, texture->gid, 0);
	}

	void drawBuffers(GLenum* buffers, GLuint num=1){
		printGLErrors("fbo drawBuffers");
		glDrawBuffers(num, buffers);
		printGLErrors("/fbo drawBuffers");
	}

	void finalize(){
		drawBuffers(&buffers[0], buffers.size());
		checkCompleteness();
	}

	bool checkCompleteness(){
		GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		char* error;
		if (fboStatus == GL_FRAMEBUFFER_COMPLETE)
			return true;
	
		switch (fboStatus){
			case GL_FRAMEBUFFER_UNDEFINED: error = "GL_FRAMEBUFFER_UNDEFINED"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: error = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: error = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: error = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"; break;
			case GL_FRAMEBUFFER_UNSUPPORTED: error = "GL_FRAMEBUFFER_UNSUPPORTED"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: error = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: error = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"; break;
			default: error = "unknown error";
			msg("Incomplete framebuffer",error);
		}
		return false;
	}

	void read(void* value, GLuint x, GLuint y, GLuint w=1, GLuint h=1, GLenum format=GL_RGBA, GLenum type=GL_FLOAT, GLenum attachment=GL_COLOR_ATTACHMENT0){
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gid);
		glReadBuffer(attachment);
		glReadPixels(x,y,w,h,format,type,value);

		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}
};

inline void bind2FB(Texture* tex){
	Framebuffer::instance().bind();
	Framebuffer::instance().attach(tex);
}


inline void unbind2FB(){
	Framebuffer::instance().unbind();
}
