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
	int jumpback;               // �Ƿ�ص�
	vector<int> excess;         // �൯�˵�����
	vector<int> omission;       // �ٵ��˵�����
	vector<int> intersection;   // ��ȷ���˵�����
};

struct BeatRhythm {
	int progress; // ���ȿ�����Ϊ1��ʾ���ˣ�0��ʾ��ȷ��-1��ʾ����
	double start; // С�ڿ�ʼʱ��
	double end;   // С�ڽ���ʱ��
	double during; // С�ڳ���ʱ�䣬������ȥ��ʼ
	int beatnum;   //�ڼ�С��
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

	vector<vector<vector<double> > > scoreEvent;        // ������Ϣ

	vector<double> scoreOnset;

	vector<vector<int>> scorePitches;

	vector<vector<vector<double>>> sfResult;

	vector<int> barFirst;                           // ����λ��
	vector<int> barEnd;                             // ��βλ��

	const int g_Pitches = 3;
	const int g_PitchesOctave = 4;
	const int g_BarFirst = 5;

};
