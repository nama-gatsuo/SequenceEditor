#pragma once
#include "ofMain.h"

struct MidiModel {
	using Ptr = std::shared_ptr<MidiModel>;

	MidiModel(bool isAttack, UCHAR velocity, UCHAR y) :
		isAttack(isAttack), velocity(velocity), y(y) {}
	
	bool isAttack;
	UCHAR velocity;
	UCHAR y;
	UCHAR level;
};

struct NoteModel {
	
	NoteModel() {}
	NoteModel(UCHAR ch, UCHAR barNum, UCHAR x, UCHAR y, UCHAR velocity, UCHAR duration) :
		ch(ch), duration(duration), barNum(barNum), x(x), y(y), velocity(velocity){}
	NoteModel(const NoteModel& n) {
		ch = n.ch;
		duration = n.duration;
		barNum = n.barNum;
		x = n.x;
		y = n.y;
		velocity = n.velocity;
		level = n.level;
	}

	UCHAR ch;
	UCHAR duration;
	UCHAR barNum;
	UCHAR x;
	UCHAR y;
	UCHAR velocity;
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