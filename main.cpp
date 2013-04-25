#include "includes.h"

Scene* Scene::current=NULL;

int main_window;
uvec2 dim(1600,900);
#define NUM_BUFFERS GLUT_DOUBLE

#include "Face.h"

int main (int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitWindowSize(dim.x,dim.y);
	glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|NUM_BUFFERS|GLUT_DEPTH);
	main_window = glutCreateWindow(title);

	//will improve error handling later
	try {
		ilInit();
		iluInit();

		GLenum err = glewInit();
		if (GLEW_OK != err)
			error("glewInit failed, aborting.");

		cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;
		cout << "OpenGL version " << glGetString(GL_VERSION) << " supported" << endl;

		srand(time(NULL));
		
		init();
		glutMotionFunc(mouseMoved);
		glutPassiveMotionFunc(passiveMouseMoved);
		glutIgnoreKeyRepeat(true);
	
		glutKeyboardUpFunc(keyUp);
		glutDisplayFunc(display);
		glutKeyboardFunc(keyDown);
		glutMouseFunc(mouseButton);
		glutReshapeFunc(reshape);
		glutIdleFunc(idle);

		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);//stop segfault on close somehow
		glutMainLoop();
	} catch (string e){
		cout << e << endl;
		pause();
	}

	return 0;
}
