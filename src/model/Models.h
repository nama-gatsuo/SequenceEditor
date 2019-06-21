#pragma once
#include "ofMain.h"

struct MidiModel {
	using Ptr = std::shared_ptr<MidiModel>;

	MidiModel(bool isAttack, unsigned char velocity, unsigned char y, unsigned char level) :
		isAttack(isAttack), velocity(velocity), y(y), level(level) {}
	
	bool isAttack;
	unsigned char velocity;
	unsigned char y;
	unsigned char level;
};

struct NoteModel {

	unsigned char ch;
	unsigned char barNum;
	unsigned char x;
	unsigned char y;
	unsigned char velocity;
	unsigned char duration;
	unsigned char level; // note layer 0 - 3

	std::array<int, 2> midiId;
};

class BarModel {
public:
	using Map = std::map<unsigned char, MidiModel::Ptr>;

	BarModel() {}

	Map& operator[](int index) {
		return notes[index];
	}

private:
	std::array<Map, 16> notes;
};