#include "ScoreLoc.h"
#include <iostream>
using namespace std;
ScoreLoc::ScoreLoc(){
	sfResult.resize(3);
}
ScoreLoc::~ScoreLoc(){

}
int ScoreLoc::Init(string scoreEventPath){
	if (ReadScoreEvent(scoreEventPath, scoreEvent) == -1) {
		cout << "Read score event error!" << endl;
		return -1;
	}
	barFirst.push_back(static_cast<int>(scoreEvent[0][g_BarFirst][0]));
	for (vector<int>::size_type i = 1; i < scoreEvent.size(); ++i) {
		if (barFirst.back() != scoreEvent[i][g_BarFirst][0]) {
			barFirst.push_back(static_cast<int>(scoreEvent[i][g_BarFirst][0]));
		}
	}
	// ��һ�ڽ�βλ�þ��ǵڶ��ڽ���λ�ü�1��barEnd[i] = barFirst[i+1]-1
	for (vector<int>::size_type i = 0; i < barFirst.size() - 1; ++i) {
		barEnd.push_back(barFirst[i + 1] - 1);
	}
	barEnd.push_back(scoreEvent.size()); // ���һ����β�������׵�λ�ø��� 

}
int ScoreLoc::ReadScoreEvent(const string &filePath, vector<vector<vector<double> > >& scoreEvent){
	ifstream fileStream(filePath);
	if (!fileStream.is_open()) {
		cout << "Error opening file scoreEvent" << endl;
		return -1; // ������-1
	}
	int linenums = 0;
	while (!fileStream.eof()){
		string strLine = "";
		getline(fileStream, strLine);
		if (strLine.empty()){
			linenums++;
		}
	}
	fileStream.close();

	ifstream scoreStream(filePath);
	if (!scoreStream.is_open()){
		cout << "Error opening file scoreEvent" << endl;
		return -1; // ������-1
	}
	if (!scoreEvent.empty()) {
		vector<vector<vector<double> > >().swap(scoreEvent);
	}
	// ���ļ��ж�ȡ����
	// ��scoreEvent��ÿһ������ȡ�ļ��е����֣��ļ���ŷ�ʽ���ļ��ĵ�һ�д��scoreEvent�ĵ�һ�У��Ӷ��õ�scoreEvent������column
	// Ȼ��һ�пո�֮��column�����֣�ÿ�������ֲ�������Ϊ��һ��
	string strLine = "";
	getline(scoreStream, strLine); // �����ļ���һ�У������ݴ�Ϊstring
	// ��string�н�����������
	istringstream iss(strLine);
	vector<double> numLine; // ����scoreEvent��һ����ֵ
	for (double d; iss >> d; numLine.push_back(d)) {}
	// ����ȡ������ִ���scoreEvent����
	vector<vector<double> > firstColumn(numLine.size());
	for (vector<int>::size_type i = 0; i < numLine.size(); ++i) {
		firstColumn[i].resize(1); // ��һ������Ԫ��ֻ��һ�������Զ�ά�����˻�Ϊһά
		firstColumn[i][0] = numLine[i];
	}
	scoreEvent.push_back(firstColumn);

	// ͨ��ͬ���ķ�ʽ����scoreEvent�ĺ�������
	for (int i = 0; i < linenums - 1; ++i) {
		getline(scoreStream, strLine); // ��ȡһ�п���
		vector<vector<double> > nColumn;
		for (vector<int>::size_type j = 0; j < numLine.size(); ++j) {
			vector<double> element; // һ����ÿ��СԪ�ض���һ������
			getline(scoreStream, strLine);
			istringstream iss2(strLine);
			for (double d; iss2 >> d; element.push_back(d)) {}
			nColumn.push_back(element); // ��ЩСԪ�ض�����һ�������ά������
		}
		scoreEvent.push_back(nColumn);
	}

	scoreStream.close();

	// ����ת��
	vector<vector<vector<double> > > scoreEventTranspose(scoreEvent[0].size());
	for (vector<int>::size_type i = 0; i < scoreEvent[0].size(); ++i) {
		scoreEventTranspose[i].resize(scoreEvent.size());
		for (vector<int>::size_type j = 0; j < scoreEvent.size(); ++j) {
			//            scoreEventTranspose[i][j].resize(scoreEvent[j][i].size());
			scoreEventTranspose[i][j] = scoreEvent[j][i];
		}
	}

	scoreEvent = scoreEventTranspose;

	// �������ֵΪ�գ����ش���
	if (scoreEvent[0][6].empty()) {
		return -1;
	}
	// ��������ֵ
	return 0;
}
void ScoreLoc::GenScoreData(string midfile){
	Options options;
	MidiFile midifile;
	vector<int> StartPitches;
	midifile.read(midfile);
	for (int event = 0; event < midifile[1].size(); event++) {
		int startmark = midifile[1][event][0];
		if (startmark != 128 && startmark != 144)
			continue;
		if (startmark == 144){
			double onset = midifile.getTimeInSeconds(midifile[1][event].tick);
			scoreOnset.push_back(onset);
			StartPitches.push_back((int)midifile[1][event][1] - 20);
		}
	}
	for (int i = 0; i < scoreOnset.size(); i++){
		int j;
		vector<int> pitch;
		for (j = i; j < scoreOnset.size(); j++){
			if (scoreOnset[i] == scoreOnset[j])
				pitch.push_back(StartPitches[j]);
			else break;
		}
		scorePitches.push_back(pitch);
		i = j;
		i--;
	}
	sort(scoreOnset.begin(), scoreOnset.end());
	vector<double>::iterator pos = unique(scoreOnset.begin(), scoreOnset.end());
	scoreOnset.erase(pos, scoreOnset.end());
	return;
}

void ScoreLoc::GensfResult(){
	for (int i = 0; i < scorePitches.size(); i++){
		sfResult[1].push_back(vector<double>{static_cast<double>(1)});
		sfResult[2].push_back(vector<double>{static_cast<double>(i + 1)});
		vector<double> result;
		for (int j = 0; j < scorePitches[i].size(); j++){
			result.push_back(scoreOnset[i]);
			result.push_back(scorePitches[i][j]);
		}
		sfResult[0].push_back(result);
	}
}


vector<Correctness> ScoreLoc::EvaluateCorrectness(){
	vector<Correctness> totalCorrectness;
	for (int i = 0; i < scorePitches.size(); i++){
		Correctness correctness;
		correctness.jumpback = 0;
		correctness.intersection = scorePitches[i];
		totalCorrectness.push_back(correctness);
	}
	return totalCorrectness;
}

vector<BeatRhythm> ScoreLoc::EvaluateBeatRhythm(){
	vector<BeatRhythm> beatRhythm(barFirst.size());
	for (int i = 0; i < barFirst.size(); i++){
		beatRhythm[i].progress = 0;
		beatRhythm[i].beatnum = i + 1;
		if (i < barFirst.size() - 1){
			beatRhythm[i].start = scoreOnset[barFirst[i] - 1];
			beatRhythm[i].end = scoreOnset[barEnd[i + 1] - 1];
		}
		else{
			beatRhythm[i].start = scoreOnset[barFirst[i] - 1];
			beatRhythm[i].end = scoreOnset[barEnd[i] - 1];
		}
		beatRhythm[i].during = beatRhythm[i].end - beatRhythm[i].start;
	}
	return beatRhythm;
}

void ScoreLoc::SaveBeatRhythm(const string& filePath, vector<BeatRhythm> beatRhythm){
	Json::Value root;
	for (vector<int>::size_type i = 0; i < beatRhythm.size(); i++){
		Json::Value data;
		data["progress"] = beatRhythm[i].progress;
		data["start"] = beatRhythm[i].start;
		data["end"] = beatRhythm[i].end;
		data["during"] = beatRhythm[i].during;
		data["beatnum"] = beatRhythm[i].beatnum;
		root["beatRhythm"].append(data);
	}
	ofstream ofs(filePath);
	ofs << root.toStyledString() << endl;
	ofs.close();
}

void ScoreLoc::SaveCorrectness(const string& filePath, vector<Correctness> correctness){
	Json::Value root;
	for (int i = 0; i < correctness.size(); i++){
		Json::Value data;
		if (i < sfResult[2].size()){
			data["position"] = static_cast<int>(sfResult[2][i][0]);
			for (int j = 0; j < sfResult[0][i].size(); j += 2){
				data["time"] = sfResult[0][i][j];
				break;
			}
		}
		else{
			data["postion"] = static_cast<int>(sfResult[2].size());
			data["time"] = "";
		}
		data["jumpback"] = correctness[i].jumpback;

		Json::Value excesses;
		Json::Value omissions;
		Json::Value intersections;
		vector<int> excessindex;
		vector<int> omissindex;
		vector<int> interindex;
		for (int j = 0; j < correctness[i].excess.size(); j++){
			excessindex.push_back(j);
		}
		for (int j = 0; j < correctness[i].omission.size(); j++){
			omissindex.push_back(j);
		}
		for (int j = 0; j < correctness[i].intersection.size(); j++){
			interindex.push_back(j);
		}
		for (int j = 0; j < correctness[i].excess.size(); j++){
			excesses[excessindex[j]] = correctness[i].excess[j];
		}
		for (int j = 0; j < correctness[i].omission.size(); j++){
			omissions[omissindex[j]] = correctness[i].omission[j];
		}
		for (int j = 0; j < correctness[i].intersection.size(); j++){
			intersections[interindex[j]] = correctness[i].intersection[j];
		}
		if (excessindex.size() == 0) excesses.resize(0);
		if (omissindex.size() == 0) omissions.resize(0);
		if (interindex.size() == 0) intersections.resize(0);

		data["excesses"] = excesses;
		data["omissions"] = omissions;
		data["intersections"] = intersections;
		root["correctness"].append(data);
	}
	ofstream ofs(filePath);
	ofs << root.toStyledString() << endl;
	ofs.close();
}


void ScoreLoc::SavePlayBack(const string& filePath){
	Json::Value root;
	for (int i = 0; i < sfResult[2].size(); i++){
		Json::Value data;
		data["isSure"] = static_cast<int>(sfResult[1][i][0]);
		data["location"] = static_cast<int>(sfResult[2][i][0]);
		Json::Value note;
		Json::Value time;
		vector<int> notelength;
		for (int j = 0; j < sfResult[0][i].size() / 2; j++){
			notelength.push_back(j);
		}
		int tx = 0, tn = 0;
		for (int j = 0; j < sfResult[0][i].size(); j += 2){
			time[notelength[tx]] = sfResult[0][i][j];
			tx++;
		}
		for (int j = 1; j < sfResult[0][i].size(); j += 2){
			note[notelength[tn]] = static_cast<int>(sfResult[0][i][j]);
			tn++;
		}
		data["time"] = time;
		data["note"] = note;
		root["playback"].append(data);
	}
	ofstream ofs(filePath);
	ofs << root.toStyledString() << endl;
	ofs.close();
}
