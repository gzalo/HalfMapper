#ifndef WAD_H
#define WAD_H

#include <vector>

//Extracted from http://hlbsp.sourceforge.net/index.php?content=waddef

struct WADHEADER{
	char szMagic[4];    // should be WAD2/WAD3
	int32_t nDir;       // number of directory entries
	int32_t nDirOffset; // offset into directory
};

struct WADDIRENTRY{
	int32_t nFilePos;            // offset in WAD
	int32_t nDiskSize;           // size in file
	int32_t nSize;               // uncompressed size
	int8_t nType;                // type of entry
	bool bCompression;           // 0 if none
	int16_t nDummy;              // not used
	char szName[16]; // must be null terminated
};

int wadLoad(const std::vector<std::string> &szGamePaths, const string &filename);

#endif
