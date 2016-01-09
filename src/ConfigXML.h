#ifndef CONFIGXML_H
#define CONFIGXML_H

#include <string>
#include <vector>
#include <map>
#include "tinyxml2.h"

#define GAMEPATH "C:/Program Files (x86)/Steam/steamapps/common/Half Life/valve"

using namespace tinyxml2;

class ConfigXML
{
public:
	ConfigXML();
	~ConfigXML();

	bool LoadProgramConfig();
	bool LoadMapConfig(const char *szFilename);

private:
	void WriteDefaultProgramConfig();

	XMLDocument  m_xmlProgramConfig;
	XMLDocument  m_xmlMapConfig;

public:
	unsigned int m_iWidth;
	unsigned int m_iHeight;
	unsigned int m_iFov;
	bool         m_bIsometric;
	std::string  m_szGamePath;


	std::map    <std::string, std::vector<std::string> > chapters;    //Vector of maps per chapter
	std::map    <std::string, std::string>               mapChapters; //Chapter of each map
	std::vector <std::string>                            maps;        //Maps in playeable order
	std::map    <std::string, bool>                      drawChapter;
	std::vector <std::string>                            wads;        //WAD files to load
};

#endif //CONFIGXML_H
