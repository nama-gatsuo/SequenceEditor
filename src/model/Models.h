#pragma once
#include "ofMain.h"

struct MidiModel {
	using Ptr = std::shared_ptr<MidiModel>;

	MidiModel(bool isAttack, UCHAR velocity, UCHAR y, UCHAR level) :
		isAttack(isAttack), velocity(velocity), y(y), level(level) {}
	
	bool isAttack;
	UCHAR velocity;
	UCHAR y;
	UCHAR level;
};

struct NoteModel {

	UCHAR ch;
	UCHAR barNum;
	UCHAR x;
	UCHAR y;
	UCHAR velocity;
	UCHAR duration;
	UCHAR level; // note layer 0 - 3

	std::array<int, 2> midiId;
};

class BarModel {
public:
	using Map = std::map<UCHAR, MidiModel::Ptr>;

	BarModel() {}

	Map& operator[](int index) {
		return notes[index];
	}

private:
	std::array<Map, 16> notes;
};