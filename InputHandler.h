#pragma once

struct InputHandler {
	bool mouseDown[2];
	bool keys[256];

	vec2 lastPt;
	vec2 mouseDelta;

	InputHandler(){
		//anyway to initialize this with initialization list?
		for (int i=0; i < 256; i++)
			keys[i] = false;
		for (int i=0; i < 2; i++)
			mouseDown[i] = false;
	}
	
	virtual void mouseButton(int button, int state, int x, int y){
		mouseDown[button==GLUT_LEFT_BUTTON?0:1] = state==GLUT_DOWN;
		if (state == GLUT_DOWN)
			lastPt = vec2(x,y);
	}
	virtual void mouseMove(int x, int y){
		vec2 curPt(x,y);
		mouseDelta = curPt - lastPt;
		lastPt = curPt;
	}
	virtual void keyDown(unsigned char key, int x, int y){
		keys[key] = true;
	}
	virtual void keyUp(unsigned char key, int x, int y){
		keys[key] = false;
	}
};

//basic first person movement through space
struct FPInput : public InputHandler, public Viewer {
	float moveSpeed;
	float sensitivity;
	vec3 vel;

	FPInput(float moveSpeed=1, float sensitivity=.1):moveSpeed(moveSpeed),sensitivity(sensitivity){

	}

	void moveForward(float dist){
		vel = forward * dist / Clock::delta;
		pos += forward * dist;
	}

	void strafeRight(float dist){
		vel = right() * dist / Clock::delta;
		pos += right() * dist;
	}
	void moveUp(float dist){
		vel = up() * dist / Clock::delta;
		pos += up() * dist;
	}

	virtual void mouseButton(int button, int state, int x, int y){
		InputHandler::mouseButton(button,state,x,y);
	}

	virtual void mouseMove(int x, int y){
		InputHandler::mouseMove(x,y);

		if (mouseDown[0])
			turn(mouseDelta.x*sensitivity,mouseDelta.y*sensitivity);
	}

	virtual void keyDown(unsigned char key, int x, int y){
		InputHandler::keyDown(key,x,y);
		switch (key){
			case '=' : moveSpeed+=1+sqrt(moveSpeed); break;
			case '-' : moveSpeed/=2; break;
		}
	}

	virtual void keyUp(unsigned char key, int x, int y){
		InputHandler::keyUp(key,x,y);
	}

	virtual void frame(){
		float move = moveSpeed * Clock::delta;
		vel = vec3(0);
		for (int i=0; i < 256; i++){
			if (keys[i])
				switch (i) {
					case 'w' : moveForward(move); break;
					case 's' : moveForward(-move);  break;
					case 'a': strafeRight(-move); break;
					case 'd': strafeRight(move); break;
					case 'e': moveUp(move); break;
					case 'q': moveUp(-move); break;
					case 'o': origin(); break;
				}
		}
	}

};
