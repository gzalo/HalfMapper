#ifndef VIDEO_H
#define VIDEO_H

#include <SDL2/SDL.h>

int videoInit(int w, int h, float fov);

extern SDL_Window *sdlWindow;
extern SDL_GLContext sdlGLContext;

#endif
