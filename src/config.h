#ifndef CONFIG_H
#define CONFIG_H

struct Config{
	int w, h, fov, isometric;
	string gamePath;
	
	map <string, vector<string> > chapters;	//Vector of maps per chapter
	map <string, string> mapChapters;	//Chapter of each map
	vector <string> maps; //Maps in playeable order
	map <string, bool> drawChapter;
	vector <string> wads; //WAD files to load
	
	Config(){
	}
	Config(const string& filename){
		w=800, h=600, fov=60, isometric=0;
		
		ifstream in;
		in.open(filename.c_str());
				
		if(!in.is_open()) return ;
		
		string chapterName;
		
		while(!in.eof()){
			string line;
			getline(in, line);
			
			unsigned int equal = line.find("=");
			
			if(equal != string::npos){
				string identifier = line.substr(0, equal);
				string value = line.substr(equal+1);
				
				if(identifier == "width"){
					stringstream ss(value);
					ss >> w;
				}else if(identifier == "height"){
					stringstream ss(value);
					ss >> h;
				}else if(identifier == "fov"){
					stringstream ss(value);
					ss >> fov;
				}else if(identifier == "gamePath"){
					gamePath = value;
					
					//Add a trailing slash if none
					if (!gamePath.empty() && *gamePath.rbegin() != '/')
						gamePath += '/';
				}else if(identifier == "wads"){
					stringstream ss(value);
					
					while(ss.good()){
						string m;
						ss >> m;
						wads.push_back(m);
					}
				}else if(identifier == "isometric"){
					stringstream ss(value);
					ss >> isometric;
				}else if(identifier == "chapter"){
					chapterName = value;
				}else if(identifier == "maps"){
					if(chapterName == ""){
						cerr << "Error in config: map(s) without chapter section" << endl;
						return;
					}
					stringstream ss(value);
					
					while(ss.good()){
						string m;
						ss >> m;
						if(m.size()>4){
							chapters[chapterName].push_back(m);
							maps.push_back(m);
							mapChapters[m] = chapterName;
						}
					}
				}else if(identifier == "drawchapter"){
					if(chapterName == ""){
						cerr << "Error in config: drawchapter without chapter section" << endl;
						return;
					}
					stringstream ss(value);
					ss >> drawChapter[chapterName];					
				}
			}			
		}

		in.close();
	}
};

#endif