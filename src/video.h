#ifndef VIDEO_H
#define VIDEO_H

typedef struct SDL_Window SDL_Window;
typedef void *SDL_GLContext;

int videoInit(int w, int h, float fov);

extern SDL_Window *sdlWindow;
extern SDL_GLContext sdlGLContext;

#endif
