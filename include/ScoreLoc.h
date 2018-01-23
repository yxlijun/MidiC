#pragma once
#include "MidiFile.h"
#include "Options.h"
#include <string>
#include <algorithm>
#include <json/json.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <cmath>

using namespace std;

struct Correctness {
	int jumpback;               // 是否回弹
	vector<int> excess;         // 多弹了的音符
	vector<int> omission;       // 少弹了的音符
	vector<int> intersection;   // 正确弹了的音符
};

struct BeatRhythm {
	int progress; // 进度快慢，为1表示快了，0表示正确，-1表示慢了
	double start; // 小节开始时间
	double end;   // 小节结束时间
	double during; // 小节持续时间，结束减去开始
	int beatnum;   //第几小节
};


class ScoreLoc
{
public:
	ScoreLoc();
	~ScoreLoc();
	int Init(string scoreEventPath);

	int ReadScoreEvent(const string &filePath, vector<vector<vector<double> > >& scoreEvent);


	void GenScoreData(string midfile);

	void GensfResult();

	vector<Correctness> EvaluateCorrectness();

	vector<BeatRhythm> EvaluateBeatRhythm();

	void SavePlayBack(const string& filePath);

	void SaveBeatRhythm(const string& filePath, vector<BeatRhythm> beatRhythm);

	void SaveCorrectness(const string& filePath, vector<Correctness> correctness);



private:

	vector<vector<vector<double> > > scoreEvent;        // 乐谱信息

	vector<double> scoreOnset;

	vector<vector<int>> scorePitches;

	vector<vector<vector<double>>> sfResult;

	vector<int> barFirst;                           // 节首位置
	vector<int> barEnd;                             // 节尾位置

	const int g_Pitches = 3;
	const int g_PitchesOctave = 4;
	const int g_BarFirst = 5;

};
