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
#ifndef CONFIGXML_H
#define CONFIGXML_H

#include <string>
#include <vector>
#include <map>
#include "tinyxml2.h"


// Set the default game paths for Half Life.
#ifdef _MSC_VER
	#define PATH_DELIM '\\'
	#define HALFLIFE_DEFAULT_GAMEPATH "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Half-Life\\valve\\"
#endif

#ifdef __linux__
	#define PATH_DELIM '/'
	#define HALFLIFE_DEFAULT_GAMEPATH "~/.steam/steamapps/common/Half-Life/valve/"
#endif

// CStrike default path is derrived from HL.
#define CSTRIKE_DEFAULT_GAMEPATH HALFLIFE_DEFAULT_GAMEPATH "cstrike"


using namespace tinyxml2;

/**
 * Stores config data for individual maps.
 */
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
};//end MapEntry


/**
 * Stores config data for chapters.
 */
struct ChapterEntry
{
	ChapterEntry()
	{
		this->m_fOffsetX = this->m_fOffsetY = this->m_fOffsetZ = 0.0f;
	}

	bool                  m_bRender;     /** Toggle renderin of this entire chapter. */
	std::string           m_szName;      /** Name of this chapter. */
	std::vector<MapEntry> m_vMapEntries; /** Vector of maps in this chapter. */
	float                 m_fOffsetX; /** Offset X axis. */
	float                 m_fOffsetY; /** Offset Y axis. */
	float                 m_fOffsetZ; /** Offset Z axis. */
};//end ChapterEntry


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
	XMLError LoadProgramConfig();

	/** 
	 * Load a map config from a given file.
	 * \param szFilename File to load from.
	 */
	XMLError LoadMapConfig(const char *szFilename);

	// User config.
	unsigned int              m_iWidth;          /** Window width. */
	unsigned int              m_iHeight;         /** Window height. */
	float                     m_fFov;            /** Field of view angle. */
	bool                      m_bIsometric;      /** Isometric rendering. */
	std::vector<std::string>  m_szGamePaths;     /** Locations of the game files. */
	// Map config.
	std::vector<ChapterEntry> m_vChapterEntries; /** Vector of chapters, containing maps. */
	std::vector<std::string>  m_vWads;           /** WAD files to load. */

private:
	/** Write the default user config when not present. */
	XMLError WriteDefaultProgramConfig();

	XMLDocument  m_xmlProgramConfig; /** Store the tinyxml2 object for the user config. */
	XMLDocument  m_xmlMapConfig;     /** Store the tinyxml2 object for the map config. */

};//end ConfigXML

#endif //CONFIGXML_H
