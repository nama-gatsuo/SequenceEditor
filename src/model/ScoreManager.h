#pragma once
#include "ofMain.h"
#include "Models.h"
#include "MidiSender.h"

class ScoreManager {
public:
	void setup(USHORT beat, USHORT barCount, USHORT channelCount);

	// for sequencer IF
	void bang(USHORT barNum, USHORT beatNum);

	// for UI IF: CRUD
	// return id
	int create(USHORT ch, USHORT barNum, const NoteModel& note);
	NoteModel& get(USHORT ch, USHORT barNum, int id);
	int update(USHORT ch, USHORT barNum, int id, const NoteModel& note);
	void remove(USHORT ch, USHORT barNum, int id);

	// setter & getter
	USHORT getBeat() { return beat; }
	USHORT getBarCount() { return barCount; }
	USHORT getChannelCount() { return channelCount; }

private:
	std::pair<USHORT, USHORT> calcEnd(USHORT startBeat, USHORT startBar, USHORT duration);

	// bar:     4 bars
	// channel: 16 instruments
	vector<vector<BarModel>> midis;
	
	// id, note
	std::unordered_map<int, NoteModel> notes;

	USHORT beat;
	USHORT barCount;
	USHORT channelCount;

	MidiSender sender;

};