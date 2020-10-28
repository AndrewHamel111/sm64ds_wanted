#ifndef FILES_H_
#define FILES_H_

/*
* @returns A std::vector of strings (filepaths) to every .png in the atlas_dir
TEMPORARILY DISABLED
string_vector ReadAtlasDir()
{
	string_vector filepaths;
	
	// verify that the resources/textures directory exists
	if(!DirectoryExists("resources/textures")) return filepaths;
	
	int count;
	char ** files = GetDirectoryFiles("resources/textures", &count);
	
	// log error if there's no files
	if (count <= 2)
		std::cerr << "No files found in resources/textures" << std::endl;
	
	// add all .pngs found in textures to filepaths
	for(int i = 2; i < count; i++)
		if (strcmp(GetExtension(files[i]), "png") == 0)
			filepaths.push_back(strcat("resources/textures/",files[i]));
		// filepaths[i].push_back("resources/snd/" + dir[i] + "/" + files[j]);
	
	// free memory from GetDirectoryFiles search
	ClearDirectoryFiles();
	
	// return the list of filepaths
	return filepaths;
}
*/

/*
* @returns A std::vector of strings (filepaths) to every .ogg in the sound_dir
*/
std::vector<string_vector> ReadSoundDir(bool* flag)
{
	// one, two, three, four
	// miss, lose, points, bgm
	std::vector<string_vector> filepaths{8};
	std::string dir[] = {"one", "two", "three", "four", "miss", "lose", "points", "bgm"};
	
	if (!DirectoryExists("resources/snd"))
	{
		*flag = true;
		return filepaths;
	}
	
	bool q = true;
	for(int i = 0; i < 8; i++) q &= DirectoryExists(("resources/snd/" + dir[i]).c_str());
	
	if (q)
		for(int i = 0; i < 8; i++)
		{
			// collect all the files
			int count;
			char** files = GetDirectoryFiles(("resources/snd/" + dir[i]).c_str(), &count);
			
			// print string if sounds are missing
			if (count <= 2) std::cerr << "No files in " << "resources/snd/" << dir[i] << std::endl;
			
			for(int j = 2; j < count; j++)
				if (strcmp(GetExtension(files[j]), "ogg") == 0)
				{
					filepaths[i].push_back("resources/snd/" + dir[i] + "/" + files[j]);
				}
				
			ClearDirectoryFiles();
		}
	else
		*flag = true;
	// else
		// throw "Sub directories not matching! directories in resources/snd/ must include\none, two, three, four, miss, lose, points, bgm";
	
	return filepaths;
}

/*
* @param atlas 
* @param textureIndex int representing the atlas to be loaded (corresponding to filepaths)
* @param filepaths std::vector<std::string> containing relative filepaths with the root as the directory of the .exe
TEMPORARILY DISABLED
void LoadAtlas(Texture2D* atlas, int* textureIndex, string_vector filepaths)
{
	// verify integrity of filepaths
	if (filepaths.empty())
	{
		std::cerr << "filepaths empty!" << std::endl;
		return;
	}
	
	// verify valid textureIndex
	if (*textureIndex >= filepaths.size() || *textureIndex < 0)
		*textureIndex = 0;
	
	*atlas = LoadTexture(filepaths[*textureIndex].c_str());
}
*/

void LoadSounds(std::vector<std::vector<Sound>>* character_sounds, std::vector<std::vector<Sound>>* sound_effects, std::vector<Music>* bgm)
{
	// retrieve list of files and verify size
	bool f = false;
	std::vector<string_vector> files = ReadSoundDir(&f);
	
	if (f)
	{
		std::cout<< "fuck man yoiu lose" << std::endl;
	}
	
	// verify character_sounds and sound_effects aren't already populated
	character_sounds->clear();
	(*character_sounds) = std::vector<std::vector<Sound>>{4};
	
	sound_effects->clear();
	(*sound_effects) = std::vector<std::vector<Sound>>{4};
	
	bgm->clear();
	
	std::vector<std::vector<Sound>>* currentVector;
	
	for(int i = 0; i < 7; i++)
	{
		if (i < 4)
		{
			int sz = files[i].size();
			for(int j = 0; j < sz; j++)
				(*character_sounds)[i].push_back(LoadSound(files[i][j].c_str()));
		}
		else
		{
		std::cout<< "sound_effects" << std::endl;
			int sz = files[i].size();
			for(int j = 0; j < sz; j++)
				(*sound_effects)[i - 4].push_back(LoadSound(files[i][j].c_str()));
		}
	}
	
	// lastly populate the music
	int _sz = files[7].size();
	for(int i = 0; i < _sz; i++)
		bgm->push_back(LoadMusicStream(files[7][i].c_str()));
	
		std::cout<< "stale cum" << std::endl;
}

void GetNames(std::vector<std::string>& names)
{
	std::string _str = getBodyFromURL("http://hamel111.myweb.cs.uwindsor.ca/sm64games/wanted/nicktober_names.csv");
	// http::Request request("http://hamel111.myweb.cs.uwindsor.ca/sm64games/wanted/nicktober_names.csv");
	// const http::Response response = request.send("GET");
	// std::string _str = std::string(response.body.begin(), response.body.end());

	names.clear();

	std::stringstream str(_str);
	std::string s;
	while (std::getline(str, s, ',')) names.push_back(s);

	std::cout << names << std::endl;
}

void DownloadScores(score_vector& scores)
{
	// new version using HTTPRequest.hpp and jute.h
	
	// retrieve string from url (getBodyFromURL(std::string))
	// http::Request request("http://hamel111.myweb.cs.uwindsor.ca/sm64games/wanted/scoreboard.json");
	// const http::Response response = request.send("GET");
	// std::string str = std::string(response.body.begin(), response.body.end());
	std::string str = getBodyFromURL("http://hamel111.myweb.cs.uwindsor.ca/sm64games/wanted/scoreboard.json");

	jute::jValue board = jute::parser::parse(str);

	std::vector<std::string> keys = board.get_keys();
	int s = keys.size();


	score_vector _scores;
	for(int i = 0; i < s; i++)
	{
		_scores.emplace_back(keys[i], static_cast<unsigned short>(board[keys[i]].as_int()) );
	}
	std::sort(_scores.begin(), _scores.end(), score_pair_sort);

	// _scores now sorted in descending order
	scores.clear();
	scores = _scores;

	std::cout << "DOWNLOADED " << scores.size() << " SCORES." << std::endl;
}

void UploadScore(score_pair score)
{	
	std::string str = "name=" + score.first + "&score=" + std::to_string(score.second);
	sendPostRequest("http://hamel111.myweb.cs.uwindsor.ca/sm64games/wanted/update.php", str);
	// try
	// {
		// std::string str = "name=" + score.first + "&score=" + std::to_string(score.second);
		
		//send post request (sendPostRequest(std::string url, std::string post))
		// http::Request request("http://hamel111.myweb.cs.uwindsor.ca/sm64games/wanted/update.php");
		// const http::Response response = request.send("POST", str, {"Content-Type: application/x-www-form-urlencoded"});
	// }
	// catch (const std::exception& e)
	// {
		// std::cout << "can't upload scores to server! fuck!\n";
	// }
}

void UpdateScores(score_vector& scores, score_pair score)
{
		// first upload the new score
	UploadScore(score);
		// then get an updated state of the scoreboard
	DownloadScores(scores);
}

void CheckVersionJSON(bool* needUpdate, bool* drawUpdateButton, Rectangle* buttonRect, std::string* url)
{
	// new version using HTTPRequest.hpp and jute.h
	
	// retrieve string from url (getBodyFromURL)
	// http::Request request("http://hamel111.myweb.cs.uwindsor.ca/sm64games/wanted/version.json");
	// const http::Response response = request.send("GET");
	// string str = std::string(response.body.begin(), response.body.end());
	std::string str = getBodyFromURL("http://hamel111.myweb.cs.uwindsor.ca/sm64games/wanted/version.json");

	jute::jValue board = jute::parser::parse(str);
	
	string_vector keys = board.get_keys();
	auto i = keys.begin(), e = keys.end();
	bool q = false;
	if (std::count(i,e,"version") < 1)
	{
		std::cerr << "Version key missing from version.json. Check jValue and server." << std::endl;
		return;
	}
	
	if (board["version"].as_double() != WANTED_VERSION)
		*needUpdate = true;
	else return;
	
	// collect all the remaining info from version.json. First, do we draw a button?
	if (std::count(i,e,"show_download_button") < 1)
	{
		// don't draw a button
		*drawUpdateButton = false;
	}
	else
	{
		// draw a button
		*drawUpdateButton = true;
		
		// also get the button Rect
		*buttonRect = Rectangle{board["button_x"].as_int(), board["button_y"].as_int(), board["button_width"].as_int(), board["button_height"].as_int()};
		
		// lastly the download page for the update
		*url = board["download_url"].as_string();
	}
}

#endif
