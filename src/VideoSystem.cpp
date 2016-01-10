/*
 * halfmapper, a renderer for GoldSrc maps and chapters.
 *
 * Copyright(C) 2014  Gonzalo Ávila "gzalo" Alterach
 * Copyright(C) 2015  Anthony "birkett" Birkett
 *
 * This file is part of halfmapper.
 *
 * This program is free software; you can redistribute it and / or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/
 */
#include <iostream>
#include <SDL.h>
#include <GL/glew.h>
#include "VideoSystem.h"

/**
 * Set the basic configuration of the window and renderer.
 * \param iWidth      Window width.
 * \param iHeight     Window height.
 * \param fFox        Field of View angle.
 * \param bFullscreen Fullscreen or Windowed mode.
 */
VideoSystem::VideoSystem(int iWidth, int iHeight, float fFov, bool bFullscreen)
{
	m_iWidth      = iWidth;
	m_iHeight     = iHeight;
	m_fFov        = fFov;
	m_bFullscreen = bFullscreen;

}//end VideoSystem::VideoSystem()


/**
 * Cleanup the renderer.
 */
VideoSystem::~VideoSystem()
{
	SDL_GL_DeleteContext(this->sdlGLContext);
	SDL_DestroyWindow(this->sdlWindow);

}//end VideoSystem::~VideoSystem()


/**
 * Create the window and GL context.
 */
int VideoSystem::Init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cerr << "Can't initialize SDL." << std::endl;
		return -1;
	}

	unsigned int windowflags = SDL_WINDOW_OPENGL;

	if (this->m_bFullscreen) {
		windowflags |= SDL_WINDOW_FULLSCREEN;
	}

	this->sdlWindow = SDL_CreateWindow("HalfMapper (loading maps, please wait)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->m_iWidth, this->m_iHeight, windowflags);

	if (this->sdlWindow == NULL) {
		std::cerr << "Can't create SDL window." << std::endl;
		return -1;
	}

	this->sdlGLContext = SDL_GL_CreateContext(this->sdlWindow);

	if (this->sdlGLContext == NULL) {
		std::cerr << "Can't create OpenGL Context." << std::endl;
		return -1;
	}

	SDL_GL_MakeCurrent(this->sdlWindow, this->sdlGLContext);

	if (glewInit() != GLEW_OK) {
		std::cerr << "Can't initialize Glew." << std::endl;
		return -1;
	}

	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_GL_SetSwapInterval(0); //Disable Vsync.

	this->SetupViewport();

	return 0;

}//end VideoSystem::Init()


/**
 * Clear the buffer.
 */
void VideoSystem::ClearBuffer()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}//end VideoSystem::ClearBuffer()


/**
 * Swap the buffers (double buffering).
 */
void VideoSystem::SwapBuffers()
{
	SDL_GL_SwapWindow(this->sdlWindow);

}//end VideoSystem::SwapBuffers()


/**
 * Set the title of the program window.
 * \param szTitle New window title.
 */
void VideoSystem::SetWindowTitle(const char* szTitle)
{
	SDL_SetWindowTitle(this->sdlWindow, szTitle);

}//end VideoSystem::SetWindowTitle()


/**
 * Set the perspective of the viewport, and set some GL hints.
 */
void VideoSystem::SetupViewport()
{
	glViewport(0, 0, this->m_iWidth, this->m_iHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
		gluPerspective(this->m_fFov, (float)this->m_iWidth / (float)this->m_iHeight, 1.0f, 100000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.8f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_TEXTURE_2D);

}//end VideoSystem::SetupViewport()
