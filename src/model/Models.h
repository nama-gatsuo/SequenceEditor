#pragma once
#include "ofMain.h"

struct MidiModel {
	using Ptr = std::shared_ptr<MidiModel>;

	MidiModel(bool isAttack, int velocity, int y, int level) :
		isAttack(isAttack), velocity(velocity), y(y), level(level) {}
	
	bool isAttack;
	
	int velocity;
	int y;
	int level;
};

struct NoteModel {
	uint8_t ch;
	uint8_t barNum;
	uint8_t x; // pos in 16 beat in 1 bar
	uint8_t y; // pitch
	uint8_t velocity;
	uint8_t duration;
	uint8_t level; // note layer 0 - 3

	std::array<int, 2> midiId;
};

class BarModel {
public:
	using Map = std::map<unsigned char, MidiModel::Ptr>;

	BarModel() {}

	Map& operator[](int index) {
		return notes[index];
	}

	const Map& operator[](int index) const {
		return notes[index];
	}

private:
	std::array<Map, 16> notes;
};