#ifndef CONFIGXML_H
#define CONFIGXML_H

#include <string>
#include <vector>
#include <map>
#include "tinyxml2.h"

#define GAMEPATH "C:/Program Files (x86)/Steam/steamapps/common/Half Life/valve"

using namespace tinyxml2;

/** Stores config data for individual maps. */
struct MapEntry
{
	MapEntry()
	{
		this->m_fOffsetX = this->m_fOffsetY = this->m_fOffsetZ = 0.0f;
	}

	bool        m_bRender;  /** Toggle rendering of this chapter. */
	std::string m_szName;   /** Map name. */
	float       m_fOffsetX; /** Offset X axis. */
	float       m_fOffsetY; /** Offset Y axis. */
	float       m_fOffsetZ; /** Offset Z axis. */
};


/** Stores config data for chapters. */
struct ChapterEntry
{
	bool                  m_bRender;     /** Toggle renderin of this entire chapter. */
	std::string           m_szName;      /** Name of this chapter. */
	std::vector<MapEntry> m_vMapEntries; /** Vector of maps in this chapter. */
};


/**
 * ConfigXML - a self-contained class for loading XML configuration files.
 */
class ConfigXML
{
public:
	/** Constructor */
	ConfigXML();

	/** Destructor */
	~ConfigXML();

	/** Load the user config from config.xml */
	bool LoadProgramConfig();

	/** 
	 * Load a map config from a given file.
	 * \param szFilename File to load from.
	 */
	bool LoadMapConfig(const char *szFilename);

private:
	/** Write the default user config when not present. */
	void WriteDefaultProgramConfig();

	XMLDocument  m_xmlProgramConfig; /** Store the tinyxml2 object for the user config. */
	XMLDocument  m_xmlMapConfig;     /** Store the tinyxml2 object for the map config. */

public:
	unsigned int m_iWidth;     /** Window width. */
	unsigned int m_iHeight;    /** Window height. */
	unsigned int m_iFov;       /** Field of view angle. */
	bool         m_bIsometric; /** Isometric rendering. */
	std::string  m_szGamePath; /** Location of the game files. */

	std::vector<ChapterEntry> m_vChapterEntries; /** Vector of chapters, containing maps. */
	std::vector<std::string>  m_vWads;           /** WAD files to load. */
};

#endif //CONFIGXML_H
