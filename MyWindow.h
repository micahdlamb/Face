#pragma once

struct MyWindow : public ViewportManager {
	
	void operator()(){
		ViewportManager::operator()();
		glEnable(GL_SCISSOR_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Clock::init();

		//experimental
		//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	void draw(){
		Clock::frame();
		ViewportManager::draw();
#if NUM_BUFFERS == 2
		glutSwapBuffers();
#else
		glFlush();
		glEnd();
#endif
	}
};