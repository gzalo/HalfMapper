#ifndef CONFIG_H
#define CONFIG_H

struct Config{
	int w, h, fov;
	string gamePath;
	
	map <string, vector<string> > chapters;	//Vector of maps per chapter
	map <string, string> mapChapters;	//Chapter of each map
	map <string, bool> drawChapter;
	
	Config(){
	}
	Config(const string& filename){
		w=800; h=600, fov=60;
		
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
				}
				if(identifier == "height"){
					stringstream ss(value);
					ss >> h;
				}
				if(identifier == "fov"){
					stringstream ss(value);
					ss >> fov;
				}
				if(identifier == "gamePath"){
					gamePath = value;
					
					//Add a trailing slash if none
					if (!gamePath.empty() && *gamePath.rbegin() != '/')
						gamePath += '/';
				}
				if(identifier == "chapter"){
					chapterName = value;
				}
				if(identifier == "maps"){
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
							mapChapters[m] = chapterName;
						}
					}
				}
				if(identifier == "drawchapter"){
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