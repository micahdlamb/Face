//should merge these eventually but its nice having the shader file hardcoded
struct Background {
	Shader shader;
	UniformSampler picture;

	void operator()(const char* pic){
		shader("background.vert","background.frag");
		shader.enable();
		picture("pic",&shader,new ILTexture(pic));
	}

	void draw(){
		shader.enable();
		//picture.push();//will be needed if I start using other textures
		Shapes::square()->draw();
	}
};

struct CubeBackground {
	Shader shader;
	UniformSampler cubeMap;

	void operator()(Texture* cubemap){
		shader("cubeBackground.vert","cubeBackground.frag");
		shader.enable();
		cubeMap("cubemap",&shader,cubemap);
	}

	void set(Texture* tex){
		shader.enable();
		cubeMap = tex;
	}

	void draw(){
		shader.enable();
		//cubeMap.push();//needed if other textures using same texture unit
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_CULL_FACE);
		Shapes::cube_background()->draw();
		glPopAttrib();
	}
};