#pragma once
#include "ofMain.h"
#include "Models.h"
#include "MidiSender.h"
#include "ChannelInfo.h"

class ScoreManager {
public:
	void setup(unsigned char beat, unsigned char barCount, unsigned char pitchCount);
	void loadJson(const string& file) {}

	// for sequencer IF
	void bang(unsigned char barNum, unsigned char beatNum);

	// for UI IF: CRUD
	// return id
	void setBar(unsigned char bar) { currentBar = bar; }
	void setChan(unsigned char ch) { currentChan = ch; }
	void setCurrent(unsigned char bar, unsigned char ch);

	int create(const NoteModel& note);
	NoteModel& get(int id);
	std::unordered_map<int, NoteModel>& get();
	std::unordered_map<int, NoteModel>& get(unsigned char bar, unsigned char ch);
	int update(int id, const NoteModel& note);
	void remove(int id);
	void remove(unsigned char bar, unsigned char ch, int id);
	//void clearCurrent(unsigned char level);

	ChannelInfo& getChannelInfo() { return chanInfos[currentChan]; }
	ChannelInfo& getChannelInfo(unsigned char index) { return chanInfos[index]; }

	void drawChannelInfo();

	// setter & getter
	unsigned char getBeat() const { return beat; }
	unsigned char getBarCount() const { return barCount; }
	unsigned char getChannelCount() const { return channelCount; }

	static unsigned char beat;
	static unsigned char barCount;
	static unsigned char channelCount;
	static unsigned char pitchCount;

	// for visual IF: get note
	vector<vector<BarModel>>& getMidis() { return midis; }
	vector<vector<std::unordered_map<int, NoteModel>>>& getNotes() { return notes; }

private:
	std::pair<unsigned char, unsigned char> calcEnd(unsigned char startBeat, unsigned char startBar, unsigned char duration) const;

	unsigned char currentBar;
	unsigned char currentChan;

	// bar:     4 bars
	// channel: 16 instruments
	vector<vector<BarModel>> midis;

	// bar:     4 bars
	// channel: 16 instruments
	// (id, note)
	vector<vector<std::unordered_map<int, NoteModel>>> notes;

	vector<ChannelInfo> chanInfos;

	MidiSender sender;

};