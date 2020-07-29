#pragma once

#include "Models.h"
#include "MidiSender.h"
#include "ChannelInfo.h"

class ScoreManager {
public:
	ScoreManager() : 
		beat(0), barCount(0), channelCount(0), pitchCount(0)
	{}

	void setup(uint8_t beat, uint8_t barCount, uint8_t pitchCount);
	void loadJson(const string& file) {}

	// for sequencer IF
	void bang(uint8_t bar, uint8_t beat);

	// for UI IF: CRUD
	
	// Create 
	int create(const NoteModel& note);
	std::unordered_map<int, NoteModel>& get(uint8_t bar, uint8_t channel);
	const NoteModel& get(uint8_t bar, uint8_t channel, int id);
	int update(uint8_t bar, uint8_t channel, int id, const NoteModel& note);
	void remove(uint8_t bar, uint8_t ch, int id);
	//void clearCurrent(unsigned char level);
	
	ChannelInfo& getChannelInfo(uint8_t index) { return chanInfos[index]; }

	void drawChannelInfo();

	// setter & getter
	uint8_t getBeatCount() const { return beat; }
	uint8_t getBarCount() const { return barCount; }
	void setBarCount(uint8_t b) { barCount = b; }

	uint8_t getChannelCount() const { return channelCount; }
	uint8_t getPitchCount() const { return pitchCount; }

	// for visual IF: get note
	std::vector<std::vector<BarModel>>& getMidis() { return midis; }
	std::vector<std::vector<std::unordered_map<int, NoteModel>>>& getNotes() { return notes; }

private:
	std::pair<uint8_t, uint8_t> calcEnd(uint8_t startBeat, uint8_t startBar, uint8_t duration) const;

	uint8_t beat;
	uint8_t barCount;
	uint8_t channelCount;
	uint8_t pitchCount;

	// bar:     4 bars
	// channel: 16 instruments
	std::vector<std::vector<BarModel>> midis;

	// bar:     4 bars
	// channel: 16 instruments
	// (id, note)
	std::vector<std::vector<std::unordered_map<int, NoteModel>>> notes;

	std::vector<ChannelInfo> chanInfos;

	MidiSender sender;

};