#include "video.h"
#include "common.h"

SDL_Window *sdlWindow;
SDL_GLContext sdlGLContext;

int videoInit(int w, int h, float fov){
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		cerr << "Can't initialize SDL." << endl;
		return -1;
	}

	sdlWindow = SDL_CreateWindow("HalfMapper (loading maps, please wait)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);

	if(sdlWindow == NULL){
		cerr << "Can't create SDL window." << endl;
		return -1;
	}

	sdlGLContext = SDL_GL_CreateContext(sdlWindow);

	if(sdlGLContext == NULL){
		cerr << "Can't create OpenGL Context." << endl;
		return -1;
	}

	SDL_GL_MakeCurrent(sdlWindow, sdlGLContext);
	
	if(glewInit() != GLEW_OK){
		cerr << "Can't initialize Glew." << endl;
		return -1;
  	}

	SDL_SetRelativeMouseMode(SDL_TRUE);

	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, (float)w/(float)h, 1.0f, 100000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	SDL_GL_SetSwapInterval(0); //Disable Vsync.

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.8f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT); 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	
	return 0;
}
