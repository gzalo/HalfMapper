#include "ConfigXML.h"

ConfigXML::ConfigXML()
{
	this->m_iWidth     = 800;
	this->m_iHeight    = 600;
	this->m_iFov       = 60;
	this->m_bIsometric = false;
}

ConfigXML::~ConfigXML()
{
	this->m_xmlProgramConfig.Clear();
	this->m_xmlMapConfig.Clear();
}

bool ConfigXML::LoadProgramConfig()
{
	XMLError eRetCode = this->m_xmlProgramConfig.LoadFile("config.xml");
	
	if (eRetCode != XML_SUCCESS) {
		this->WriteDefaultProgramConfig();
		return true;
	}


	XMLNode *rootNode = this->m_xmlProgramConfig.FirstChild();

	XMLElement *width = rootNode->FirstChildElement("width");
	XMLElement *height = rootNode->FirstChildElement("height");
	XMLElement *fov = rootNode->FirstChildElement("fov");
	XMLElement *isometric = rootNode->FirstChildElement("isometric");
	XMLElement *gamepath = rootNode->FirstChildElement("gamepath");

	width->QueryUnsignedText(&this->m_iWidth);
	height->QueryUnsignedText(&this->m_iHeight);
	fov->QueryUnsignedText(&this->m_iFov);
	isometric->QueryBoolText(&this->m_bIsometric);
	this->m_szGamePath = gamepath->GetText();

	//Add a trailing slash if none
	if (!this->m_szGamePath.empty() && *this->m_szGamePath.rbegin() != '/')
		this->m_szGamePath += '/';
	
	return true;

}

bool ConfigXML::LoadMapConfig(const char *szFilename)
{
	this->m_xmlMapConfig.LoadFile(szFilename);

	XMLNode *rootNode = this->m_xmlMapConfig.FirstChild();

	XMLElement *wadsElement = rootNode->FirstChildElement("wads");

	XMLElement *wad = wadsElement->FirstChildElement("wad");

	while (wad != nullptr) {
		this->m_vWads.push_back(wad->GetText());
		wad = wad->NextSiblingElement("wad");
	}



	XMLElement *chapter = rootNode->FirstChildElement("chapter");

	while (chapter != nullptr) {
		ChapterEntry sChapterEntry;
		sChapterEntry.m_szName  = chapter->Attribute("name");
		sChapterEntry.m_bRender = chapter->BoolAttribute("render");

		XMLElement *map = chapter->FirstChildElement("map");

		while (map != nullptr) {
			MapEntry sMapEntry;
			sMapEntry.m_szName  = map->Attribute("name");
			sMapEntry.m_bRender = map->BoolAttribute("render");

			map->QueryFloatAttribute("x", &sMapEntry.m_fOffsetX);
			map->QueryFloatAttribute("y", &sMapEntry.m_fOffsetY);
			map->QueryFloatAttribute("z", &sMapEntry.m_fOffsetZ);

			sChapterEntry.m_vMapEntries.push_back(sMapEntry);

			map = map->NextSiblingElement("map");
		}
			
		chapter = chapter->NextSiblingElement("chapter");
		this->m_vChapterEntries.push_back(sChapterEntry);
	}

	return true;
}

void ConfigXML::WriteDefaultProgramConfig()
{
	XMLNode *rootNode = this->m_xmlProgramConfig.NewElement("config");
	this->m_xmlProgramConfig.InsertFirstChild(rootNode);
	
	XMLElement *width = this->m_xmlProgramConfig.NewElement("width");
	width->SetText(this->m_iWidth);
	rootNode->InsertEndChild(width);

	XMLElement *height = this->m_xmlProgramConfig.NewElement("height");
	height->SetText(this->m_iHeight);
	rootNode->InsertEndChild(height);

	XMLElement *fov = this->m_xmlProgramConfig.NewElement("fov");
	fov->SetText(this->m_iFov);
	rootNode->InsertEndChild(fov);

	XMLElement *isometric = this->m_xmlProgramConfig.NewElement("isometric");
	isometric->SetText(this->m_bIsometric);
	rootNode->InsertEndChild(isometric);

	XMLElement *gamepath = this->m_xmlProgramConfig.NewElement("gamepath");
	gamepath->SetText(GAMEPATH);
	rootNode->InsertEndChild(gamepath);

	XMLError eRetCode = this->m_xmlProgramConfig.SaveFile("config.xml");
}
