#pragma once;

struct TexturedSquare : public Shader {
	UniformSampler tex;
	void operator()(Texture* texture){
		Shader::operator()("texturedSquare.vert","texturedSquare.frag");
		tex("tex",this,texture);
	}

	void draw(){
		Shader::enable();
		Shapes::square()->draw();
	}

	void set(Texture* texture){
		Shader::enable();
		tex = texture;
	}
};

struct TextureViewer : public Shader {
	UniformSampler tex;
	TextureViewer(Texture* texture){
		Shader::operator()("texturedSquare.vert","texturedSquare.frag");
		tex("tex",this,texture,false);
	}
	void set(Texture* texture){
		Shader::enable();
		tex = texture;
	}
	void draw(){
		Shader::enable();
		Shapes::square()->draw();
	}
	void draw(float x, float y, float w, float h){
		Viewport::push(x,y,w,h);
		glDisable(GL_DEPTH_TEST);
		draw();
		Viewport::pop();
	}
};

//hack to get around dependency loop
inline void draw(Texture* tex, float x, float y, float w, float h){
	static TextureViewer* tv = NULL;
	if (tv == NULL)
		tv = new TextureViewer(tex);
	tv->set(tex);
	tv->draw(x,y,w,h);
}