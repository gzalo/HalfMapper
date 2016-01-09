#include "common.h"
#include "video.h"
#include "wad.h"
#include "bsp.h"
#include "ConfigXML.h"

int main(int argc, char **argv){
	ConfigXML *xmlconfig = new ConfigXML();

	xmlconfig->LoadProgramConfig();

	if(argc >= 2){
		xmlconfig->LoadMapConfig(argv[1]);
	}else{
		xmlconfig->LoadMapConfig("halflife.xml");
	}
	
	if(videoInit(xmlconfig->m_iWidth, xmlconfig->m_iHeight, xmlconfig->m_iFov) == -1) return -1;

	//Texture loading
	for(size_t i=0;i<xmlconfig->wads.size();i++){
		if(wadLoad(xmlconfig->m_szGamePath + xmlconfig->wads[i]) == -1) return -1;
	}

	//Map loading
	vector <BSP*> maps;
	
	int t = SDL_GetTicks(), mapAmount=0;
	int totalTris=0;
	
	for(unsigned int i=0;i<xmlconfig->maps.size();i++){
		BSP *b = new BSP(xmlconfig->m_szGamePath + "maps/" + xmlconfig->maps[i] + ".bsp", xmlconfig->maps[i]); 
		totalTris += b->totalTris;
		maps.push_back(b);
		mapAmount++;
	}
	
	cout << mapAmount << " maps loaded in " << SDL_GetTicks()-t << " ms." << endl;
	cout << "Total triangles: " << totalTris << endl;

	//---
	
	bool quit=false;
	int kw=0, ks=0, ka=0, kd=0, kq=0, ke=0, kr=0, kc=0;
	float position[3] = {0.0f, 0.0f, 0.0f};
	float rotation[2] = {0.0f, 0.0f};
	float isoBounds=1000.0;
	int oldMs = SDL_GetTicks(), frame=0;
	
	while(!quit){
		SDL_Event event;
		while(SDL_PollEvent(&event)){
			if(event.type==SDL_QUIT) quit=true;
			if(event.type==SDL_KEYDOWN){
				if(event.key.keysym.sym == SDLK_ESCAPE) quit=true;
				if(event.key.keysym.sym == SDLK_w) kw=1;
				if(event.key.keysym.sym == SDLK_s) ks=1;
				if(event.key.keysym.sym == SDLK_a) ka=1;
				if(event.key.keysym.sym == SDLK_d) kd=1;
				if(event.key.keysym.sym == SDLK_q) kq=1;
				if(event.key.keysym.sym == SDLK_e) ke=1;
				
				if(event.key.keysym.sym == SDLK_LSHIFT) kr=1;
				if(event.key.keysym.sym == SDLK_LCTRL) kc=1;
			}
			if(event.type==SDL_KEYUP){
				if(event.key.keysym.sym == SDLK_w) kw=0;
				if(event.key.keysym.sym == SDLK_s) ks=0;
				if(event.key.keysym.sym == SDLK_a) ka=0;
				if(event.key.keysym.sym == SDLK_d) kd=0;
				if(event.key.keysym.sym == SDLK_q) kq=0;
				if(event.key.keysym.sym == SDLK_e) ke=0;
				
				if(event.key.keysym.sym == SDLK_LSHIFT) kr=0;
				if(event.key.keysym.sym == SDLK_LCTRL) kc=0;
			}
			if(event.type == SDL_MOUSEMOTION){
				rotation[0] += event.motion.xrel/15.0;
				rotation[1] += event.motion.yrel/15.0;
			}
		}

		//Clamp mouse aim
		if(rotation[0] > 360) rotation[0] -= 360.0;
		if(rotation[0] < 0) rotation[0] += 360.0;
		if(rotation[1] < -89) rotation[1] = -89;
		if(rotation[1] > 89) rotation[1] = 89;
		
		//Velocities
		int vsp = kr?32:(kc?2:8), hsp = kr?32:(kc?2:8);
		
		int m_left = 0, m_frontal = 0;
			
		if(kw) m_frontal++;
		if(ks) m_frontal--;
		if(ka) m_left++;
		if(kd) m_left--;
		
		if(xmlconfig->m_bIsometric){
			position[2] += m_left * hsp;
			position[1] += m_frontal * hsp;
			
			if(ke) isoBounds += vsp * 10.0;
			if(kq) isoBounds -= vsp * 10.0;
			isoBounds = max(10.0f, isoBounds);
		}else{
			if(m_frontal || m_left){
				float rotationF = rotation[0]* M_PI / 180.0f + atan2(m_frontal, m_left);
				position[0] -= hsp * cos(rotationF);
				position[2] -= hsp * sin(rotationF);
			}
			if(ke) position[1] += vsp;
			if(kq) position[1] -= vsp;
		}
		
		//Camera setup
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		if(xmlconfig->m_bIsometric){
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-isoBounds, isoBounds, -isoBounds, isoBounds, -100000.0, 100000.0);
		  
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glScalef(1.0f,2.0f,1.0f);
			glRotatef(rotation[1], 1.0f, 0.0f, 0.0f);
			glRotatef(rotation[0], 0.0f, 1.0f, 0.0f);
			glTranslatef(-position[0], -position[1], -position[2]);
		}else{
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glRotated(rotation[1], 1.0f, 0.0f, 0.0f);
			glRotated(rotation[0], 0.0f, 1.0f, 0.0f);		
			glTranslatef(-position[0], -position[1], -position[2]);
		}
		//Map render
		for(size_t i=0;i<maps.size();i++){
			if(xmlconfig->drawChapter[xmlconfig->mapChapters[maps[i]->getId()]])
				maps[i]->render();	
		}

		//SDL_Delay(1);
		SDL_GL_SwapWindow(sdlWindow);

		frame++;
		if(frame==30){
			frame=0;
			
			//FPS calculation
			int dt = SDL_GetTicks()-oldMs;
			oldMs = SDL_GetTicks();
			char bf[64];
			sprintf(bf, "%.2f FPS - %.2f %.2f %.2f", 30000.0f/(float)dt, position[0], position[1], position[2]);
			SDL_SetWindowTitle(sdlWindow, bf);
		}
	}
	SDL_Quit();
	
	return 0;
}
