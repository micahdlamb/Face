#pragma once
//will make general later
struct DepthBlurrer : public Shader {
	Framebuffer fb1, fb2;
	Texture* primary;
	Texture tmp;

	Uniform2f coordScale;
	UniformSampler texture;

	void operator()(Texture* tex){
		Shader::operator()("blur.vert","blur.frag");
		primary = tex;
		tmp(primary->width,primary->height, primary->internalFormat);
		coordScale("coordScale",this);
		texture("tex", this, primary);
		fb1(primary);
		fb2(&tmp);
	}

	void blur(){
		Viewport::push(0,0,primary->width, primary->height);
		glDisable(GL_DEPTH_TEST);
		Shader::enable();
		coordScale = vec2(0,1.0f/primary->height);
		texture = primary;
		fb2.bind();
		Shapes::square()->draw();
		fb2.unbind();
		coordScale = vec2(1.0f/primary->width, 0);
		texture = &tmp;
		fb1.bind();
		Shapes::square()->draw();
		fb1.unbind();
		Viewport::pop();
		texture = primary;//leave texture unit as it was
	}
};