#include "video.h"
#include "common.h"

int videoInit(int w, int h, float fov){
	putenv("SDL_VIDEO_CENTERED=1");
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		cerr << "Can't initialize SDL." << endl;
		return -1;
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL,0);
	if(SDL_SetVideoMode(w,h,32,SDL_OPENGL | SDL_HWSURFACE ) == NULL){
		cerr << "Can't create OpenGL window." << endl;
		return -1;
	}
	
	if(glewInit() != GLEW_OK){
		cerr << "Can't initialize Glew." << endl;
		return -1;
	}
	
	SDL_WM_SetCaption("HalfMapper (loading maps, please wait)",NULL);
	SDL_WarpMouse(w/2,h/2);
	SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_ShowCursor(SDL_DISABLE);
	
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, (float)w/(float)h, 1.0f, 100000.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.8);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT); 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	
	return 0;
}
