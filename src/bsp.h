#ifndef BSP_H
#define BSP_H

#include "common.h"

//Extracted from http://hlbsp.sourceforge.net/index.php?content=bspdef

#define LUMP_ENTITIES      0
#define LUMP_PLANES        1
#define LUMP_TEXTURES      2
#define LUMP_VERTICES      3
#define LUMP_VISIBILITY    4
#define LUMP_NODES         5
#define LUMP_TEXINFO       6
#define LUMP_FACES         7
#define LUMP_LIGHTING      8
#define LUMP_CLIPNODES     9
#define LUMP_LEAVES       10
#define LUMP_MARKSURFACES 11
#define LUMP_EDGES        12
#define LUMP_SURFEDGES    13
#define LUMP_MODELS       14
#define HEADER_LUMPS      15
#define MAXTEXTURENAME 16
#define MIPLEVELS 4

struct MapEntry; // Dont include ConfigXML.h here.

struct BSPLUMP{
	int32_t nOffset; // File offset to data
	int32_t nLength; // Length of data
};
struct BSPHEADER{
	int32_t nVersion;           // Must be 30 for a valid HL BSP file
	BSPLUMP lump[HEADER_LUMPS]; // Stores the directory of lumps
};
struct BSPFACE{
	uint16_t iPlane;          // Plane the face is parallel to
	uint16_t nPlaneSide;      // Set if different normals orientation
	uint32_t iFirstEdge;      // Index of the first surfedge
	uint16_t nEdges;          // Number of consecutive surfedges
	uint16_t iTextureInfo;    // Index of the texture info structure
	uint8_t nStyles[4];       // Specify lighting styles
	uint32_t nLightmapOffset; // Offsets into the raw lightmap data
};
struct BSPEDGE{
	uint16_t iVertex[2]; // Indices into vertex array
};
struct BSPTEXTUREINFO{
	VERTEX vS; 
	float fSShift;    // Texture shift in s direction
	VERTEX vT; 
	float fTShift;    // Texture shift in t direction
	uint32_t iMiptex; // Index into textures array
	uint32_t nFlags;  // Texture flags, seem to always be 0
};
struct BSPTEXTUREHEADER{
	uint32_t nMipTextures; // Number of BSPMIPTEX structures
};
struct BSPMIPTEX{
	char szName[MAXTEXTURENAME];  // Name of texture
	uint32_t nWidth, nHeight;     // Extends of the texture
	uint32_t nOffsets[MIPLEVELS]; // Offsets to texture mipmaps BSPMIPTEX;
};

#define MAX_MAP_HULLS 4
struct BSPMODEL{
    float nMins[3], nMaxs[3];          // Defines bounding box
    VERTEX vOrigin;                  // Coordinates to move the // coordinate system
    int32_t iHeadnodes[MAX_MAP_HULLS]; // Index into nodes array
    int32_t nVisLeafs;                 // ???
    int32_t iFirstFace, nFaces;        // Index and count into faces
};
 
struct COORDS{
	float u, v;
};
struct VECFINAL{
	float x,y,z,u,v,ul,vl;
	VECFINAL(float _x, float _y, float _z, float _u, float _v){
		x = _x;
		y = _y;
		z = _z;
		u = _u;
		v = _v;
	}
	VECFINAL(VERTEX vt, COORDS c, COORDS c2){
		x = vt.x, y = vt.y, z = vt.z;
		u = c.u, v = c.v;
		ul = c2.u;
		vl = c2.v;
	}
};
struct TEXTURE{
	GLuint texId;
	int w,h;
};
struct LMAP{
	unsigned char *offset; int w,h;
	int finalX, finalY;
};

struct TEXSTUFF{
	vector <VECFINAL> triangles;
	int texId;
};

class BSP{
	public:
		BSP(const std::vector<std::string> &szGamePaths, const string &filename, const MapEntry &sMapEntry);
		void render();
		int totalTris;
		void SetChapterOffset(const float x, const float y, const float z);
	private:
		void calculateOffset();

		unsigned char *lmapAtlas; GLuint lmapTexId;
		map <string, TEXSTUFF > texturedTris;
		GLuint *bufObjects;
		string mapId;
		VERTEX offset;

		VERTEX ConfigOffsetChapter;
};

extern map <string, TEXTURE> textures;
extern map <string, vector<pair<VERTEX,string> > > landmarks;
extern map <string, vector<string> > dontRenderModel;

#endif
