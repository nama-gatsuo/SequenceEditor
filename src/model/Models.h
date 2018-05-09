#pragma once
#include "ofMain.h"

struct MidiModel {
	using Ptr = std::shared_ptr<MidiModel>;

	MidiModel(bool isAttack, USHORT velocity, USHORT pitch) : isAttack(isAttack), velocity(velocity), pitch(pitch) {}
	bool isAttack;
	USHORT velocity;
	USHORT pitch;
};

struct NoteModel {
	
	NoteModel() {}
	NoteModel(USHORT ch, USHORT barNum, USHORT beatNum, USHORT pitch, USHORT velocity, USHORT duration) :
		ch(ch), duration(duration), barNum(barNum), beatNum(beatNum), pitch(pitch), velocity(velocity){}
	NoteModel(const NoteModel& n) {
		ch = n.ch;
		duration = n.duration;
		barNum = n.barNum;
		beatNum = n.beatNum;
		pitch = n.pitch;
		velocity = n.velocity;
	}

	USHORT ch;
	USHORT duration;
	USHORT barNum;
	USHORT beatNum;
	USHORT pitch;
	USHORT velocity;

	struct MidiSet {
		vector<MidiModel::Ptr>::const_iterator it;
	} midi[2];
};

class BarModel {
public:
	BarModel() {}

	vector<MidiModel::Ptr>& operator[](int index) {
		return notes[index];
	}

private:
	std::array<vector<MidiModel::Ptr>, 16> notes;
};