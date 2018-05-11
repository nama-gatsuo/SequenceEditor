#pragma once
#include "ofMain.h"

struct MidiModel {
	using Ptr = std::shared_ptr<MidiModel>;

	MidiModel(bool isAttack, USHORT velocity, USHORT y) : isAttack(isAttack), velocity(velocity), y(y) {}
	bool isAttack;
	USHORT velocity;
	USHORT y;
};

struct NoteModel {
	
	NoteModel() {}
	NoteModel(USHORT ch, USHORT barNum, USHORT x, USHORT y, USHORT velocity, USHORT duration) :
		ch(ch), duration(duration), barNum(barNum), x(x), y(y), velocity(velocity){}
	NoteModel(const NoteModel& n) {
		ch = n.ch;
		duration = n.duration;
		barNum = n.barNum;
		x = n.x;
		y = n.y;
		velocity = n.velocity;
	}

	USHORT ch;
	USHORT duration;
	USHORT barNum;
	USHORT x;
	USHORT y;
	USHORT velocity;

	std::array<int, 2> midiId;
};

class BarModel {
public:
	using Map = std::map<USHORT, MidiModel::Ptr>;

	BarModel() {}

	Map& operator[](int index) {
		return notes[index];
	}

private:
	std::array<Map, 16> notes;
};