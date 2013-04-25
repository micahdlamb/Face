#pragma once

struct Clock {
	static int timeMilli;
	static float time;
	static int deltaMilli;
	static float delta;
	static float maxDelta;

	static float start;
	static int counter;

	//unnecessary but first frame will be wrong without it
	static void init(float maxDelta=1.f/30){
		Clock::maxDelta = maxDelta;
		timeMilli = glutGet(GLUT_ELAPSED_TIME);
		frame();
	}

	static void frame(){
		int old = timeMilli;
		timeMilli = glutGet(GLUT_ELAPSED_TIME);
		time = timeMilli / 1000.f;
		deltaMilli = timeMilli - old;
		//delta = deltaMilli / 1000.f;
		delta = std::min(maxDelta, deltaMilli / 1000.f);
	
		counter++;

	}

	static float fps(){
		float r = counter / (time - start);
		counter = 0;
		start = time;
		return r;
	}

	static void printFps(int howOften){
		if (counter > howOften)
			cout << "fps: " << fps() << endl;
	}

};