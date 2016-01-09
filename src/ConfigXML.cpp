#include <iostream>
#include "ConfigXML.h"

/**
 * Constructor to set some default values.
 *
 * These values are used as fallbacks when the user config.xml can't be loaded.
 * WriteDefaultProgramConfig() will use these.
 */
ConfigXML::ConfigXML()
{
	this->m_iWidth     = 800;
	this->m_iHeight    = 600;
	this->m_iFov       = 60;
	this->m_bIsometric = false;

}//end ConfigXML::ConfigXML()


/**
 * Destructor to clean up the internal data storage.
 */
ConfigXML::~ConfigXML()
{
	this->m_xmlProgramConfig.Clear();
	this->m_xmlMapConfig.Clear();
	this->m_vWads.clear();
	this->m_vChapterEntries.clear();

}//end ConfigXML::~ConfigXML()


/**
 * Load the user (program) configuraion.
 */
XMLError ConfigXML::LoadProgramConfig()
{
	XMLError eRetCode = this->m_xmlProgramConfig.LoadFile("config.xml");
	
	if (eRetCode != XML_SUCCESS) {
		if (eRetCode == XML_ERROR_FILE_NOT_FOUND) {
			this->WriteDefaultProgramConfig();
		}
		else {
			std::cout << "Error loading config.xml. Code " << eRetCode << std::endl;
			return eRetCode;
		}
	}

	XMLNode *rootNode = this->m_xmlProgramConfig.FirstChild();

	if (rootNode == nullptr) {
		std::cout << "Malformed XML. No root element." << std::endl;
		return XML_ERROR_FILE_READ_ERROR;
	}

	XMLElement *window = rootNode->FirstChildElement("window");
	window->QueryUnsignedAttribute("width",  &this->m_iWidth);
	window->QueryUnsignedAttribute("height", &this->m_iHeight);
	window->QueryUnsignedAttribute("fov",    &this->m_iFov);
	window->QueryBoolAttribute("isometric",  &this->m_bIsometric);


	XMLElement *gamepaths = rootNode->FirstChildElement("gamepaths");

	XMLElement *gamepath = gamepaths->FirstChildElement("gamepath");

	while (gamepath != nullptr) {
		std::string path = gamepath->GetText();

		// Add a trailing slash if none.
		if (!path.empty() && *path.rbegin() != PATH_DELIM)
			path += PATH_DELIM;

		this->m_szGamePaths.push_back(path);
		gamepath = gamepath->NextSiblingElement("map");
	}

	return XML_SUCCESS;

}//end ConfigXML::LoadProgramConfig()


/**
 * Load the map configuration from a given file name.
 * \param szFilename File to load from.
 */
XMLError ConfigXML::LoadMapConfig(const char *szFilename)
{
	XMLError eRetCode = this->m_xmlMapConfig.LoadFile(szFilename);

	if (eRetCode != XML_SUCCESS) {
		std::cout << "Error loading map config. Code " << eRetCode << std::endl;
		return eRetCode;
	}

	XMLNode *rootNode = this->m_xmlMapConfig.FirstChild();

	if (rootNode == nullptr) {
		std::cout << "Malformed XML. No root element." << std::endl;
		return XML_ERROR_FILE_READ_ERROR;
	}

	XMLElement *wadsElement = rootNode->FirstChildElement("wads");

	if (wadsElement == nullptr) {
		std::cout << "Malformed XML. No wad container element." << std::endl;
		return XML_ERROR_FILE_READ_ERROR;
	}

	XMLElement *wad = wadsElement->FirstChildElement("wad");

	if (wad == nullptr) {
		std::cout << "Malformed XML. No wads element." << std::endl;
		return XML_ERROR_FILE_READ_ERROR;
	}

	while (wad != nullptr) {
		this->m_vWads.push_back(wad->GetText());
		wad = wad->NextSiblingElement("wad");
	}

	
	XMLElement *chapter = rootNode->FirstChildElement("chapter");

	if (chapter == nullptr) {
		std::cout << "Malformed XML. No chapters found." << std::endl;
		return XML_ERROR_FILE_READ_ERROR;
	}
	
	while (chapter != nullptr) {
		ChapterEntry sChapterEntry;
		sChapterEntry.m_szName  = chapter->Attribute("name");
		sChapterEntry.m_bRender = chapter->BoolAttribute("render");

		if (sChapterEntry.m_szName == "") {
			std::cout << "Malformed XML. Chapter found without name attribute." << std::endl;
			return XML_ERROR_FILE_READ_ERROR;
		}

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

	return XML_SUCCESS;

}//end LoadMapConfig()


/**
 * Write the default program config when not found.
 */
XMLError ConfigXML::WriteDefaultProgramConfig()
{
	// Document root node.
	XMLNode *rootNode = this->m_xmlProgramConfig.NewElement("config");
	
	// Window settings.
	XMLElement *window = this->m_xmlProgramConfig.NewElement("window");
	window->SetAttribute("width",     this->m_iWidth);
	window->SetAttribute("height",    this->m_iHeight);
	window->SetAttribute("fov",       this->m_iFov);
	window->SetAttribute("isometric", this->m_bIsometric);

	// Collection of game paths.
	XMLElement *gamepaths = this->m_xmlProgramConfig.NewElement("gamepaths");

	// Half Life game path.
	XMLElement *gamepath = this->m_xmlProgramConfig.NewElement("gamepath");
	gamepath->SetAttribute("name", "halflife");
	gamepath->SetText(HALFLIFE_DEFAULT_GAMEPATH);

	// Add elements to the document.
	this->m_xmlProgramConfig.InsertFirstChild(rootNode);
		rootNode->InsertFirstChild(window);
		rootNode->InsertEndChild(gamepaths);
			gamepaths->InsertFirstChild(gamepath);

	// Save.
	XMLError eRetCode = this->m_xmlProgramConfig.SaveFile("config.xml");

	if (eRetCode != XML_SUCCESS) {
		std::cout << "Error writing config.xml. Code " << eRetCode << std::endl;
		return XML_ERROR_FILE_READ_ERROR;
	}

	return XML_SUCCESS;

}//end ConfigXML::WriteDefaultProgramConfig()
