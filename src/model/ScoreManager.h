#pragma once
#include "ofMain.h"
#include "Models.h"
#include "MidiSender.h"
#include "ChannelInfo.h"

class ScoreManager {
public:
	void setup(USHORT beat, USHORT barCount, USHORT pitchCount);
	void loadJson(const string& file) {}

	// for sequencer IF
	void bang(USHORT barNum, USHORT beatNum);

	// for UI IF: CRUD
	// return id
	void setBar(USHORT bar) { currentBar = bar; }
	void setChan(USHORT ch) { currentChan = ch; }
	void setCurrent(USHORT bar, USHORT ch);
	int create(const NoteModel& note);
	NoteModel& get(int id);
	std::unordered_map<int, NoteModel>& get();
	int update(int id, const NoteModel& note);
	void remove(int id);
	
	void drawChannelInfo();

	// setter & getter
	USHORT getBeat() const { return beat; }
	USHORT getBarCount() const { return barCount; }
	USHORT getChannelCount() const { return channelCount; }

	static USHORT beat;
	static USHORT barCount;
	static USHORT channelCount;
	static USHORT pitchCount;

private:
	std::pair<USHORT, USHORT> calcEnd(USHORT startBeat, USHORT startBar, USHORT duration) const;

	USHORT currentBar;
	USHORT currentChan;

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