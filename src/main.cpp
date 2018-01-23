#include <iostream>
#include "ScoreLoc.h"
#include <io.h>
using namespace std;
void GetAllFileFromPath(string folderPath);
vector<string> datfiles;
vector<string> midifiles;
vector<string> datpaths;
int main() {
	string path = "../data/test";
	GetAllFileFromPath(path);
	string arraysavefile[3] = { "/beatRhythm.json", "/correctness.json", "/playback.json" };
	for (int i = 0; i < datfiles.size(); i++){
		string beatRhythmPath = datpaths[i] + arraysavefile[0];
		string correctnessPath = datpaths[i] + arraysavefile[1];
		string playbackPath = datpaths[i] + arraysavefile[2];

		ScoreLoc scoreFollowing;
		if (scoreFollowing.Init(datfiles[i]) == -1) {
			cout << "Read scoreEvent error!" << endl;
			exit(-1);
		}
		scoreFollowing.GenScoreData(midifiles[i]);
		scoreFollowing.GensfResult();
		vector<Correctness> correctness = scoreFollowing.EvaluateCorrectness();
		vector<BeatRhythm> beatRhythm = scoreFollowing.EvaluateBeatRhythm();
		scoreFollowing.SaveBeatRhythm(beatRhythmPath, beatRhythm);
		scoreFollowing.SaveCorrectness(correctnessPath, correctness);
		scoreFollowing.SavePlayBack(playbackPath);
	}
	system("pause");
    return 0;
}


void GetAllFileFromPath(string folderPath){
	_finddata_t FileInfo;
	string strfind = folderPath + "/*";
	intptr_t  Handle = _findfirst(strfind.c_str(), &FileInfo);
	if (Handle == -1L){
		_findclose(Handle);
		return;
	}
	do{
		if (FileInfo.attrib & _A_SUBDIR){
			if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0)){
				string newPath = folderPath + "/" + FileInfo.name;
				GetAllFileFromPath(newPath);
			}
		}
		else{
			string newPath = folderPath + "/" + FileInfo.name;
			string path = FileInfo.name;
			int index = path.find(".dat", 0);
			int index2 = path.find("mid", 0);
			if (index != -1){
				datfiles.push_back(newPath);
				datpaths.push_back(folderPath);
			}
			if (index2 != -1){
				midifiles.push_back(newPath);
			}
		}
	} while (_findnext(Handle, &FileInfo) == 0);
	_findclose(Handle);
}
