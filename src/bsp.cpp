#include "common.h"
#include "bsp.h"
#include "entities.h"
#include "ConfigXML.h"
#include <cstring>

map <string, TEXTURE> textures;
map <string, vector<pair<VERTEX,string> > > landmarks;
map <string, vector<string> > dontRenderModel;
map <string, VERTEX> offsets;

//Correct UV coordinates
static inline COORDS calcCoords(VERTEX v, VERTEX vs, VERTEX vt, float sShift, float tShift){
	COORDS ret;
	ret.u = sShift + vs.x*v.x + vs.y*v.y + vs.z*v.z; 
	ret.v = tShift + vt.x*v.x + vt.y*v.y + vt.z*v.z;
	return ret;
}

BSP::BSP(const std::vector<std::string> &szGamePaths, const string &filename, const MapEntry &sMapEntry){
	string id = sMapEntry.m_szName;

	uint8_t gammaTable[256];
	for(int i=0;i<256;i++)
		gammaTable[i] = pow(i/255.0,1.0/3.0)*255;

	//Light map atlas
	lmapAtlas = new uint8_t[1024*1024*3];

	ifstream inBSP;

	// Try to open the file from all known gamepaths.
	for (size_t i = 0; i < szGamePaths.size(); i++) {
		if (!inBSP.is_open()) {
			inBSP.open(szGamePaths[i] + filename.c_str(), ios::binary);
		}
	}

	// If the BSP wasn't found in any of the gamepaths...
	if(!inBSP.is_open()){ cerr << "Can't open BSP " << filename << "." << endl; return;}
	
	//Check BSP version
	BSPHEADER bHeader;
	inBSP.read((char*)&bHeader, sizeof(bHeader));
	if(bHeader.nVersion != 30){ cerr << "BSP version is not 30 (" << filename << ")." << endl; return;}
	
	//Read Entities
	inBSP.seekg(bHeader.lump[LUMP_ENTITIES].nOffset, ios::beg);
	char *bff = new char[bHeader.lump[LUMP_ENTITIES].nLength];
	inBSP.read(bff, bHeader.lump[LUMP_ENTITIES].nLength);
	parseEntities(bff,id,sMapEntry);
	delete []bff;
	
	//Read Models and hide some faces
	BSPMODEL *models = new BSPMODEL[bHeader.lump[LUMP_MODELS].nLength/(int)sizeof(BSPMODEL)];
	inBSP.seekg(bHeader.lump[LUMP_MODELS].nOffset, ios::beg);	
	inBSP.read((char*)models, bHeader.lump[LUMP_MODELS].nLength);
	
	map <int, bool> dontRenderFace;
	for(unsigned int i=0;i<dontRenderModel[id].size();i++){
		int modelId = atoi(dontRenderModel[id][i].substr(1).c_str());
		int startingFace = models[modelId].iFirstFace;
		for(int j=0;j<models[modelId].nFaces;j++){
			//if(modelId == 57) cout << j+startingFace << endl;
			dontRenderFace[j+startingFace] = true;
		}
	}	
	
	
	//Read Vertices
	vector <VERTEX> vertices;
	inBSP.seekg(bHeader.lump[LUMP_VERTICES].nOffset, ios::beg);		
	for(int i=0;i<bHeader.lump[LUMP_VERTICES].nLength/12;i++){
		VERTEX v;
		inBSP.read((char*)&v, sizeof(v));
		vertices.push_back(v);
	}
	
	//Read Edges	
	BSPEDGE *edges = new BSPEDGE[bHeader.lump[LUMP_EDGES].nLength/(int)sizeof(BSPEDGE)];
	inBSP.seekg(bHeader.lump[LUMP_EDGES].nOffset, ios::beg);	
	inBSP.read((char*)edges, bHeader.lump[LUMP_EDGES].nLength);
	
	//Read Surfedges
	vector <VERTEX> verticesPrime;
	inBSP.seekg(bHeader.lump[LUMP_SURFEDGES].nOffset, ios::beg);
	for(int i=0;i<bHeader.lump[LUMP_SURFEDGES].nLength/(int)sizeof(int);i++){
		int e;
		inBSP.read((char*)&e, sizeof(e));
		verticesPrime.push_back(vertices[edges[e>0?e:-e].iVertex[e>0?0:1]]);
	}
	
	//Read Lightmaps
	inBSP.seekg(bHeader.lump[LUMP_LIGHTING].nOffset, ios::beg);
	int size = bHeader.lump[LUMP_LIGHTING].nLength;
	uint8_t *lmap = new uint8_t[size];
	inBSP.read((char*)lmap, size);
	vector <LMAP> lmaps;
	
	//Read Textures
	inBSP.seekg(bHeader.lump[LUMP_TEXTURES].nOffset, ios::beg);
	BSPTEXTUREHEADER theader;
	inBSP.read((char*)&theader, sizeof(theader));
	int *texOffSets = new int[theader.nMipTextures];
	inBSP.read((char*)texOffSets, theader.nMipTextures*sizeof(int));
	
	vector <string> texNames;
	
	for(unsigned int i=0;i<theader.nMipTextures;i++){
		inBSP.seekg(bHeader.lump[LUMP_TEXTURES].nOffset+texOffSets[i], ios::beg);
		
		BSPMIPTEX bmt;
		inBSP.read((char*)&bmt, sizeof(bmt));
		if(textures.count(bmt.szName) == 0){ //First appearance of the texture
			if(bmt.nOffsets[0] != 0 && bmt.nOffsets[1] != 0 && bmt.nOffsets[2] != 0 && bmt.nOffsets[3] != 0){
				//Textures that are inside the BSP
				
				//Awful code. This and wad.cpp may be joined, they are pretty similar (except that these don't have color palettes)
				
				unsigned char *data0 = new unsigned char[bmt.nWidth*bmt.nHeight];
				inBSP.seekg(bHeader.lump[LUMP_TEXTURES].nOffset+texOffSets[i]+bmt.nOffsets[0], ios::beg);
				inBSP.read((char*)data0, bmt.nWidth*bmt.nHeight);
				
				unsigned char *data1 = new unsigned char[bmt.nWidth*bmt.nHeight/4];
				inBSP.seekg(bHeader.lump[LUMP_TEXTURES].nOffset+texOffSets[i]+bmt.nOffsets[1], ios::beg);
				inBSP.read((char*)data1, bmt.nWidth*bmt.nHeight/4);
				
				unsigned char *data2 = new unsigned char[bmt.nWidth*bmt.nHeight/16];
				inBSP.seekg(bHeader.lump[LUMP_TEXTURES].nOffset+texOffSets[i]+bmt.nOffsets[2], ios::beg);
				inBSP.read((char*)data2, bmt.nWidth*bmt.nHeight/16);
				
				unsigned char *data3 = new unsigned char[bmt.nWidth*bmt.nHeight/64];
				inBSP.seekg(bHeader.lump[LUMP_TEXTURES].nOffset+texOffSets[i]+bmt.nOffsets[3], ios::beg);
				inBSP.read((char*)data3, bmt.nWidth*bmt.nHeight/64);
				
				short dummy; inBSP.read((char*)&dummy, 2);
				
				unsigned char *data4 = new unsigned char[256*3];
				inBSP.read((char*)data4, 256*3);
				
				unsigned char *dataFinal0 = new unsigned char[bmt.nWidth*bmt.nHeight*4];
				unsigned char *dataFinal1 = new unsigned char[bmt.nWidth*bmt.nHeight];
				unsigned char *dataFinal2 = new unsigned char[bmt.nWidth*bmt.nHeight/4];
				unsigned char *dataFinal3 = new unsigned char[bmt.nWidth*bmt.nHeight/16];
				
				for(unsigned int y=0;y<bmt.nHeight;y++)
				for(unsigned int x=0;x<bmt.nWidth;x++){
					dataFinal0[(x+y*bmt.nWidth)*4] = data4[data0[y*bmt.nWidth+x]*3];
					dataFinal0[(x+y*bmt.nWidth)*4+1] = data4[data0[y*bmt.nWidth+x]*3+1];
					dataFinal0[(x+y*bmt.nWidth)*4+2] = data4[data0[y*bmt.nWidth+x]*3+2];
					
					if(dataFinal0[(x+y*bmt.nWidth)*4] == 0 && dataFinal0[(x+y*bmt.nWidth)*4+1] == 0 && dataFinal0[(x+y*bmt.nWidth)*4+2] == 255)
						dataFinal0[(x+y*bmt.nWidth)*4+3] = dataFinal0[(x+y*bmt.nWidth)*4+2] = dataFinal0[(x+y*bmt.nWidth)*4+1] = dataFinal0[(x+y*bmt.nWidth)*4+0] = 0;
					else
						dataFinal0[(x+y*bmt.nWidth)*4+3] = 255;
				}
				for(unsigned int y=0;y<bmt.nHeight/2;y++)
				for(unsigned int x=0;x<bmt.nWidth/2;x++){
					dataFinal1[(x+y*bmt.nWidth/2)*4] = data4[data1[y*bmt.nWidth/2+x]*3];
					dataFinal1[(x+y*bmt.nWidth/2)*4+1] = data4[data1[y*bmt.nWidth/2+x]*3+1];
					dataFinal1[(x+y*bmt.nWidth/2)*4+2] = data4[data1[y*bmt.nWidth/2+x]*3+2];
					
					if(dataFinal1[(x+y*bmt.nWidth/2)*4] == 0 && dataFinal1[(x+y*bmt.nWidth/2)*4+1] == 0 && dataFinal1[(x+y*bmt.nWidth/2)*4+2] == 255)
						dataFinal1[(x+y*bmt.nWidth/2)*4+3] = dataFinal1[(x+y*bmt.nWidth/2)*4+2] = dataFinal1[(x+y*bmt.nWidth/2)*4+1] = dataFinal1[(x+y*bmt.nWidth/2)*4+0] = 0;
					else
						dataFinal1[(x+y*bmt.nWidth/2)*4+3] = 255;
				}
				for(unsigned int y=0;y<bmt.nHeight/4;y++)
				for(unsigned int x=0;x<bmt.nWidth/4;x++){
					dataFinal2[(x+y*bmt.nWidth/4)*4] = data4[data2[y*bmt.nWidth/4+x]*3];
					dataFinal2[(x+y*bmt.nWidth/4)*4+1] = data4[data2[y*bmt.nWidth/4+x]*3+1];
					dataFinal2[(x+y*bmt.nWidth/4)*4+2] = data4[data2[y*bmt.nWidth/4+x]*3+2];
					
					if(dataFinal2[(x+y*bmt.nWidth/4)*4] == 0 && dataFinal2[(x+y*bmt.nWidth/4)*4+1] == 0 && dataFinal2[(x+y*bmt.nWidth/4)*4+2] == 255)
						dataFinal2[(x+y*bmt.nWidth/4)*4+3] = dataFinal2[(x+y*bmt.nWidth/4)*4+2] = dataFinal2[(x+y*bmt.nWidth/4)*4+1] = dataFinal2[(x+y*bmt.nWidth/4)*4+0] = 0;
					else
						dataFinal2[(x+y*bmt.nWidth/4)*4+3] = 255;
				}
				for(unsigned int y=0;y<bmt.nHeight/8;y++)
				for(unsigned int x=0;x<bmt.nWidth/8;x++){
					dataFinal3[(x+y*bmt.nWidth/8)*4] = data4[data3[y*bmt.nWidth/8+x]*3];
					dataFinal3[(x+y*bmt.nWidth/8)*4+1] = data4[data3[y*bmt.nWidth/8+x]*3+1];
					dataFinal3[(x+y*bmt.nWidth/8)*4+2] = data4[data3[y*bmt.nWidth/8+x]*3+2];
					
					if(dataFinal3[(x+y*bmt.nWidth/8)*4] == 0 && dataFinal3[(x+y*bmt.nWidth/8)*4+1] == 0 && dataFinal3[(x+y*bmt.nWidth/8)*4+2] == 255)
						dataFinal3[(x+y*bmt.nWidth/8)*4+3] = dataFinal3[(x+y*bmt.nWidth/8)*4+2] = dataFinal3[(x+y*bmt.nWidth/8)*4+1] = dataFinal3[(x+y*bmt.nWidth/8)*4+0] = 0;
					else
						dataFinal3[(x+y*bmt.nWidth/8)*4+3] = 255;
				}
				
				TEXTURE n;
				n.w = bmt.nWidth; n.h = bmt.nHeight;
				glGenTextures(1, &n.texId);		
				glBindTexture(GL_TEXTURE_2D, n.texId);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmt.nWidth  , bmt.nHeight  , 0, GL_RGBA, GL_UNSIGNED_BYTE, dataFinal0);
				glTexImage2D(GL_TEXTURE_2D, 1, GL_RGBA, bmt.nWidth/2, bmt.nHeight/2, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataFinal1);
				glTexImage2D(GL_TEXTURE_2D, 2, GL_RGBA, bmt.nWidth/4, bmt.nHeight/4, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataFinal2);
				glTexImage2D(GL_TEXTURE_2D, 3, GL_RGBA, bmt.nWidth/8, bmt.nHeight/8, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataFinal3);
				textures[bmt.szName]=n;
				
				delete[] data0; delete[] data1; delete[] data2; delete[] data3; delete[] data4;
				delete[] dataFinal0; delete[] dataFinal1; delete[] dataFinal2; delete[] dataFinal3;
				
			}else{
				TEXTURE n;
				n.texId = 0;
				n.w = 1;
				n.h = 1;
				textures[bmt.szName]=n;
			}
		}
		texNames.push_back(bmt.szName);
	}
	
	//Read Texture information
	inBSP.seekg(bHeader.lump[LUMP_TEXINFO].nOffset, ios::beg);
	BSPTEXTUREINFO *btfs = new BSPTEXTUREINFO[bHeader.lump[LUMP_TEXINFO].nLength/(int)sizeof(BSPTEXTUREINFO)];
	inBSP.read((char*)btfs, bHeader.lump[LUMP_TEXINFO].nLength);	
	
	//Read Faces and lightmaps
	inBSP.seekg(bHeader.lump[LUMP_FACES].nOffset, ios::beg);
	
	float *minUV = new float[bHeader.lump[LUMP_FACES].nLength/(int)sizeof(BSPFACE)*2];
	float *maxUV = new float[bHeader.lump[LUMP_FACES].nLength/(int)sizeof(BSPFACE)*2];
	
	for(int i=0;i<bHeader.lump[LUMP_FACES].nLength/(int)sizeof(BSPFACE);i++){
		BSPFACE f;
		inBSP.read((char*)&f, sizeof(f));
		BSPTEXTUREINFO b = btfs[f.iTextureInfo];
		string faceTexName = texNames[b.iMiptex];
		
		minUV[i*2] = minUV[i*2+1] = 99999;
		maxUV[i*2] = maxUV[i*2+1] = -99999;
		
		for(int j=2,k=1;j<f.nEdges;j++,k++){	
			VERTEX v1 = verticesPrime[f.iFirstEdge], v2 = verticesPrime[f.iFirstEdge+k], v3 = verticesPrime[f.iFirstEdge+j];
			COORDS c1 = calcCoords(v1, b.vS, b.vT, b.fSShift, b.fTShift),
				   c2 = calcCoords(v2, b.vS, b.vT, b.fSShift, b.fTShift),
				   c3 = calcCoords(v3, b.vS, b.vT, b.fSShift, b.fTShift);
			
			minUV[i*2] = min(minUV[i*2], c1.u); minUV[i*2+1] = min(minUV[i*2+1], c1.v);
			minUV[i*2] = min(minUV[i*2], c2.u); minUV[i*2+1] = min(minUV[i*2+1], c2.v);
			minUV[i*2] = min(minUV[i*2], c3.u); minUV[i*2+1] = min(minUV[i*2+1], c3.v);
			
			maxUV[i*2] = max(maxUV[i*2], c1.u); maxUV[i*2+1] = max(maxUV[i*2+1], c1.v);
			maxUV[i*2] = max(maxUV[i*2], c2.u); maxUV[i*2+1] = max(maxUV[i*2+1], c2.v);
			maxUV[i*2] = max(maxUV[i*2], c3.u); maxUV[i*2+1] = max(maxUV[i*2+1], c3.v);
		}
		
		int lmw = ceil(maxUV[i*2]/16) - floor(minUV[i*2]/16) + 1;
		int lmh = ceil(maxUV[i*2+1]/16) - floor(minUV[i*2+1]/16) + 1;
		
		if(lmw > 17 || lmh > 17) lmw = lmh = 1;
		LMAP l; l.w = lmw; l.h = lmh; l.offset = lmap+f.nLightmapOffset;
		lmaps.push_back(l);
	}
		
	int lmapRover[1024]; memset(lmapRover, 0, 1024*4);

	//Light map "rover" algorithm from Quake 2 (http://fabiensanglard.net/quake2/quake2_opengl_renderer.php)
	for(unsigned int i=0;i<lmaps.size();i++){
		int best=1024, best2;

		for(int a=0;a<1024-lmaps[i].w;a++){
			best2 = 0;
			int j;
			for(j=0;j<lmaps[i].w;j++){
				if(lmapRover[a+j] >= best)
					break;
				if(lmapRover[a+j] > best2)
					best2 = lmapRover[a+j];
			}
			if(j == lmaps[i].w){
				lmaps[i].finalX = a;
				lmaps[i].finalY = best = best2;
			}
		}

		if(best + lmaps[i].h > 1024){
			cout << "Lightmap atlas is too small (" << filename <<")." << endl;
			break;
		}

		for(int a=0;a<lmaps[i].w;a++)
			lmapRover[lmaps[i].finalX + a] = best + lmaps[i].h;
		
		int finalX = lmaps[i].finalX;
		int finalY = lmaps[i].finalY;
		
		#define ATXY(_x,_y) ((_x)+((_y)*1024))*3
		#define LMXY(_x,_y) ((_x)+((_y)*lmaps[i].w))*3
		for(int y=0;y<lmaps[i].h;y++)
		for(int x=0;x<lmaps[i].w;x++){
			lmapAtlas[ATXY(finalX+x, finalY+y)+0] = gammaTable[lmaps[i].offset[LMXY(x,y)+0]];
			lmapAtlas[ATXY(finalX+x, finalY+y)+1] = gammaTable[lmaps[i].offset[LMXY(x,y)+1]];
			lmapAtlas[ATXY(finalX+x, finalY+y)+2] = gammaTable[lmaps[i].offset[LMXY(x,y)+2]];
		}
	}

	//Load the actual triangles
	
	inBSP.seekg(bHeader.lump[LUMP_FACES].nOffset, ios::beg);
	for(int i=0;i<bHeader.lump[LUMP_FACES].nLength/(int)sizeof(BSPFACE);i++){
		
		
		BSPFACE f;
		inBSP.read((char*)&f, sizeof(f));
		
		if(dontRenderFace[i]) continue;
		
		BSPTEXTUREINFO b = btfs[f.iTextureInfo];
		
		string faceTexName = texNames[b.iMiptex];
		
		//Calculate light map uvs
		int lmw = ceil(maxUV[i*2]/16) - floor(minUV[i*2]/16) + 1;
		int lmh = ceil(maxUV[i*2+1]/16) - floor(minUV[i*2+1]/16) + 1;
		
		if(lmw > 17) continue;
		if(lmh > 17) continue;
		
		float mid_poly_s = (minUV[i*2] + maxUV[i*2])/2.0f;
		float mid_poly_t = (minUV[i*2+1] + maxUV[i*2+1])/2.0f;
		float mid_tex_s = (float)lmw / 2.0f;
		float mid_tex_t = (float)lmh / 2.0f;
		float fX = lmaps[i].finalX;
		float fY = lmaps[i].finalY;
		TEXTURE t = textures[faceTexName];
		
		vector <VECFINAL>*vt = &texturedTris[faceTexName].triangles;
		
		for(int j=2,k=1;j<f.nEdges;j++,k++){	
			VERTEX v1 = verticesPrime[f.iFirstEdge], v2 = verticesPrime[f.iFirstEdge+k], v3 = verticesPrime[f.iFirstEdge+j];
			COORDS c1 = calcCoords(v1, b.vS, b.vT, b.fSShift, b.fTShift),
				   c2 = calcCoords(v2, b.vS, b.vT, b.fSShift, b.fTShift),
				   c3 = calcCoords(v3, b.vS, b.vT, b.fSShift, b.fTShift);
			
			COORDS c1l, c2l, c3l;
						
			c1l.u = mid_tex_s + (c1.u - mid_poly_s) / 16.0f;
			c2l.u = mid_tex_s + (c2.u - mid_poly_s) / 16.0f;
			c3l.u = mid_tex_s + (c3.u - mid_poly_s) / 16.0f;
			c1l.v = mid_tex_t + (c1.v - mid_poly_t) / 16.0f;
			c2l.v = mid_tex_t + (c2.v - mid_poly_t) / 16.0f;
			c3l.v = mid_tex_t + (c3.v - mid_poly_t) / 16.0f;
			
			c1l.u += fX; c2l.u += fX; c3l.u += fX;
			c1l.v += fY; c2l.v += fY; c3l.v += fY;
			
			c1l.u /= 1024.0; c2l.u /= 1024.0; c3l.u /= 1024.0;
			c1l.v /= 1024.0; c2l.v /= 1024.0; c3l.v /= 1024.0;
			
			c1.u /= t.w; c2.u /= t.w; c3.u /= t.w;
			c1.v /= t.h; c2.v /= t.h; c3.v /= t.h;
			
			v1.fixHand();
			v2.fixHand();
			v3.fixHand();
			
			vt->push_back(VECFINAL(v1,c1,c1l));
			vt->push_back(VECFINAL(v2,c2,c2l));
			vt->push_back(VECFINAL(v3,c3,c3l));
		}
		texturedTris[faceTexName].texId = textures[faceTexName].texId;
	}

	
	delete []btfs;
	delete []texOffSets;
	delete []edges;
	
	inBSP.close();
	
	glGenTextures(1, &lmapTexId);		
	glBindTexture(GL_TEXTURE_2D, lmapTexId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, lmapAtlas);
	delete []lmapAtlas;
	
	bufObjects = new GLuint[texturedTris.size()];
	glGenBuffers(texturedTris.size(), bufObjects);
	
	int i=0;
	totalTris=0;
	for(map <string, TEXSTUFF >::iterator it = texturedTris.begin();it != texturedTris.end();it++, i++){	
		glBindBuffer(GL_ARRAY_BUFFER, bufObjects[i]);
		glBufferData(GL_ARRAY_BUFFER, (*it).second.triangles.size()*sizeof(VECFINAL), (void*)&(*it).second.triangles[0], GL_STATIC_DRAW);
		totalTris += (*it).second.triangles.size();
	}
	
	mapId = id;
		
}

void BSP::calculateOffset(){
	if(offsets.count(mapId) != 0){
		offset = offsets[mapId];
	}else{
		if(mapId == "c0a0"){
			//Origin for other maps
			offsets[mapId] = VERTEX(0,0,0);
		}else{
			float ox=0,oy=0,oz=0;
			bool found=false;
			for(map <string, vector<pair<VERTEX,string> > >::iterator it = landmarks.begin(); it != landmarks.end();it++){
				if((*it).second.size() > 1){
					for(size_t i=0;i<(*it).second.size();i++){
						if((*it).second[i].second == mapId){
							if(i == 0){
								if(offsets.count((*it).second[i+1].second) != 0){
									VERTEX c1 = (*it).second[i].first;
									VERTEX c2 = (*it).second[i+1].first;
									VERTEX c3 = offsets[(*it).second[i+1].second];
									ox = + c2.x + c3.x - c1.x;
									oy = + c2.y + c3.y - c1.y;
									oz = + c2.z + c3.z - c1.z;
									
									found=true;
									cout << "Matched " << (*it).second[i].second << " " << (*it).second[i+1].second << endl;
									break;
								}
							}else{
								if(offsets.count((*it).second[i-1].second) != 0){
									VERTEX c1 = (*it).second[i].first;
									VERTEX c2 = (*it).second[i-1].first;
									VERTEX c3 = offsets[(*it).second[i-1].second];
									ox = + c2.x + c3.x - c1.x;
									oy = + c2.y + c3.y - c1.y;
									oz = + c2.z + c3.z - c1.z;
									
									found=true;
									cout << "Matched " << (*it).second[i].second << " " << (*it).second[i-1].second << endl;
									break;
								}
							}
							
						}
					}
				}
			}
			if(!found){
				cout << "Cant find matching landmarks for " << mapId << endl;  
			}
			offsets[mapId] = VERTEX(ox,oy,oz);
		}
	}
}

void BSP::render(){
	//Calculate map offset based on landmarks
	calculateOffset();
	
	glPushMatrix();
	glTranslatef(offset.x + ConfigOffsetChapter.x, offset.y + ConfigOffsetChapter.y, offset.z + ConfigOffsetChapter.z);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glActiveTextureARB(GL_TEXTURE1_ARB); 
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, lmapTexId);

	glClientActiveTextureARB(GL_TEXTURE1_ARB); 
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);	
	
	int i=0;
	for(map <string, TEXSTUFF >::iterator it = texturedTris.begin();it != texturedTris.end();it++, i++){
		//Don't render some dummy triangles (triggers and such)
		if((*it).first != "aaatrigger" && (*it).first != "origin" && (*it).first != "clip" && (*it).first != "sky" && (*it).first[0]!='{' && (*it).second.triangles.size() != 0){
			//if(mapId == "c1a0e.bsp") cout << (*it).first << endl;
			glBindBuffer(GL_ARRAY_BUFFER, bufObjects[i]);
			
			/////T0
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, (*it).second.texId);
			
			glClientActiveTextureARB(GL_TEXTURE0_ARB); 
			glTexCoordPointer(2, GL_FLOAT, sizeof(VECFINAL), (char*)NULL+4*3);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY); 
			
			////T1
			glClientActiveTextureARB(GL_TEXTURE1_ARB); 
			glTexCoordPointer(2, GL_FLOAT, sizeof(VECFINAL), (char*)NULL+4*5);
			
			glVertexPointer(3, GL_FLOAT, sizeof(VECFINAL), (void*)0);
			glDrawArrays(GL_TRIANGLES, 0, (*it).second.triangles.size());
		}
	}
	glPopMatrix();
}

void BSP::SetChapterOffset(const float x, const float y, const float z)
{
	ConfigOffsetChapter.x = x;
	ConfigOffsetChapter.y = y;
	ConfigOffsetChapter.z = z;
}
