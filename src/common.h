#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <algorithm>
#include <assert.h>
#include <SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <sys/types.h>

#ifndef _MSC_VER
	#include <dirent.h>
#endif

using namespace std;

struct VERTEX{
	float x,y,z;
	void fixHand(){
		float swapY = y; 
		y = z;
		z = swapY;
		x = -x;
	}
	VERTEX(float _x, float _y, float _z){
		x=_x; y=_y; z=_z;
	}
	VERTEX(){
	}
};

#endif
