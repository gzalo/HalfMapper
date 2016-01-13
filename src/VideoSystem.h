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
#ifndef VIDEO_H
#define VIDEO_H

// Avoid including SDL just for these basic types.
typedef struct SDL_Window SDL_Window;
typedef void *SDL_GLContext;


/**
 * Rendering and Window management.
 */
class VideoSystem
{
public:
	/**
	 * Set the basic configuration of the window and renderer.
	 * \param iWidth         Window width.
	 * \param iHeight        Window height.
	 * \param fFox           Field of View angle.
	 * \param bFullscreen    Fullscreen or Windowed mode.
	 * \param bMultisampling Enable or disable multisampling.
	 * \param bVsync         Enable or disable Vsync.
	 */
	VideoSystem(int iWidth, int iHeight, float fFov, bool bFullscreen, bool bMultisampling, bool bVsync);

	/** Destructor */
	~VideoSystem();

	/** Create the window and GL context. */
	int  Init();

	/** Clear the buffer. */
	void ClearBuffer();

	/** Swap the buffers (double buffering). */
	void SwapBuffers();

	/**
	 * Set the title of the program window.
	 * \param szTitle New window title.
	 */
	void SetWindowTitle(const char *szTitle);

private:
	/** Set the perspective of the viewport, and set some GL hints. */
	void SetupViewport();

	/**
	 * Enable or disable multisampling for the GL context.
	 * \param bEnable Enable or Disable.
	 */
	void SetMultisampling(bool bEnable);

	/**
	* Enable or disable Vsync for the GL context.
	* \param bEnable Enable or Disable.
	*/
	void SetVsync(bool bEnable);

	unsigned int  m_iWidth;         /** Window width. */
	unsigned int  m_iHeight;        /** Window height. */
	float         m_fFov;           /** Field of View angle. */
	bool          m_bFullscreen;    /** Fullscreen or Windowed mode. */
	bool          m_bMultisampling; /** Multisampling enable or disable. */
	bool          m_bVsync;         /** Vsync enable or disable. */
	SDL_Window*   sdlWindow;        /** Pointer to the SDL Window. */
	SDL_GLContext sdlGLContext;     /** Hold the SDL OpenGL Context. */

};//end VideoSystem

#endif //VIDEO_H
