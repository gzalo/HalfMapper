#include "bsp.h"
#include "wad.h"

int wadLoad(const std::vector<std::string> &szGamePaths, const string &filename) {
	ifstream inWAD;

	// Try to open the file from all known gamepaths.
	for (size_t i = 0; i < szGamePaths.size(); i++) {
		if (!inWAD.is_open()) {
			inWAD.open(szGamePaths[i] + filename.c_str(), ios::binary);
		}
	}

	// If the WAD wasn't found in any of the gamepaths...
	if(!inWAD.is_open()){ cerr << "Can't load WAD " << filename << "." << endl; return -1; }
	
	//Read header
	WADHEADER wh; inWAD.read((char*)&wh, sizeof(wh));
	if(wh.szMagic[0] != 'W' || wh.szMagic[1] != 'A' || wh.szMagic[2] != 'D' || wh.szMagic[3] != '3') return -1;
	
	//Read directory entries
	WADDIRENTRY *wdes = new WADDIRENTRY[wh.nDir];
	inWAD.seekg(wh.nDirOffset, ios::beg);		
	inWAD.read((char*)wdes, sizeof(WADDIRENTRY)*wh.nDir);
	
	uint8_t *dataDr = new uint8_t[512*512];	 //Raw texture data 
	uint8_t *dataUp = new uint8_t[512*512*4]; //32 bit texture 
	uint8_t *dataPal = new uint8_t[256*3];	//256 color pallete
	
	for(int i=0;i<wh.nDir;i++){
		inWAD.seekg(wdes[i].nFilePos, ios::beg);

		BSPMIPTEX bmt;
		inWAD.read((char*)&bmt, sizeof(bmt));
		if(textures.count(bmt.szName) == 0){ //Only load if it's the first appearance of the texture
				
			TEXTURE n;
			n.w = bmt.nWidth; n.h = bmt.nHeight;
			glGenTextures(1, &n.texId);		
			glBindTexture(GL_TEXTURE_2D, n.texId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);
			
			//Sizes of each mipmap
			const int dimensionsSquared[4] = {1,4,16,64};
			const int dimensions[4] = {1,2,4,8};
			
			//Read each mipmap
			for(int mip=3;mip>=0;mip--){
				inWAD.seekg(wdes[i].nFilePos+bmt.nOffsets[mip], ios::beg);
				inWAD.read((char*)dataDr, bmt.nWidth*bmt.nHeight/dimensionsSquared[mip]);
			
				if(mip == 3){
					//Read the pallete (comes after last mipmap)
					uint16_t dummy; inWAD.read((char*)&dummy, 2);
					inWAD.read((char*)dataPal, 256*3);
				}
				
				for(uint32_t y=0;y<bmt.nHeight/dimensions[mip];y++)
				for(uint32_t x=0;x<bmt.nWidth/dimensions[mip];x++){
					dataUp[(x+y*bmt.nWidth/dimensions[mip])*4] = dataPal[dataDr[y*bmt.nWidth/dimensions[mip]+x]*3];
					dataUp[(x+y*bmt.nWidth/dimensions[mip])*4+1] = dataPal[dataDr[y*bmt.nWidth/dimensions[mip]+x]*3+1];
					dataUp[(x+y*bmt.nWidth/dimensions[mip])*4+2] = dataPal[dataDr[y*bmt.nWidth/dimensions[mip]+x]*3+2];
					
					//Do full transparency on blue pixels
					if(dataUp[(x+y*bmt.nWidth/dimensions[mip])*4] == 0 && dataUp[(x+y*bmt.nWidth/dimensions[mip])*4+1] == 0 && dataUp[(x+y*bmt.nWidth/dimensions[mip])*4+2] == 255)
						dataUp[(x+y*bmt.nWidth/dimensions[mip])*4+3] = 0;
					else
						dataUp[(x+y*bmt.nWidth/dimensions[mip])*4+3] = 255;
				}
				
				glTexImage2D(GL_TEXTURE_2D, mip, GL_RGBA, bmt.nWidth/dimensions[mip], bmt.nHeight/dimensions[mip], 0, GL_RGBA, GL_UNSIGNED_BYTE, dataUp);
			}
		
			textures[bmt.szName]=n;				
		}
	}
	
	delete []dataDr; delete []dataUp; delete []dataPal; delete []wdes;
	return 0;
}
