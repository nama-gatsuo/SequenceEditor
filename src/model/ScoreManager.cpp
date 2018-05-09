#include "ScoreManager.h"

USHORT ScoreManager::beat = 0;
USHORT ScoreManager::barCount = 0;
USHORT ScoreManager::channelCount = 0;
USHORT ScoreManager::pitchCount = 0;

void ScoreManager::setup(USHORT beat, USHORT barCount, USHORT channelCount, USHORT pitchCount) {
	this->beat = beat;
	this->barCount = barCount;
	this->channelCount = channelCount;
	this->pitchCount = pitchCount;

	midis.assign(this->barCount, vector<BarModel>(this->channelCount));
	notes.assign(this->barCount, vector<std::unordered_map<int, NoteModel>>(this->channelCount));
}

void ScoreManager::bang(USHORT barNum, USHORT beatNum) {

	unsigned short ch = 0;
	for (BarModel& bar : midis[barNum]) {
		for (MidiModel::Ptr& note : bar[beatNum]) {

			if (note->isAttack) sender.sendMidiOn(ch, note->pitch, note->velocity);
			else sender.sendMidiOff(ch, note->pitch);

		}
		ch++;
	}

}

int ScoreManager::create(USHORT ch, USHORT barNum, const NoteModel& note) {
	
	auto& s = midis[barNum][ch][note.beatNum];
	s.push_back(std::make_shared<MidiModel>(true, note.velocity, note.pitch));
	
	auto& beatAndBar = calcEnd(note.beatNum, note.barNum, note.duration);
	auto& e = midis[beatAndBar.second][ch][beatAndBar.first];
	e.push_back(std::make_shared<MidiModel>(false, 0, note.pitch));
	
	int lastId = notes[barNum][ch].size();
	notes[barNum][ch].emplace(lastId, note);
	notes[barNum][ch][lastId].midi[0].it = s.cbegin() + s.size() - 1;
	notes[barNum][ch][lastId].midi[1].it = e.cbegin() + e.size() - 1;

	return lastId;
}

NoteModel& ScoreManager::get(USHORT ch, USHORT barNum, int id) {
	return notes[barNum][ch][id];
}

std::unordered_map<int, NoteModel>& ScoreManager::get(USHORT ch, USHORT barNum) {
	return notes[barNum][ch];
}

int ScoreManager::update(USHORT ch, USHORT barNum, int id, const NoteModel& note) {
	remove(ch, barNum, id);
	return create(ch, barNum, note);
}

void ScoreManager::remove(USHORT ch, USHORT barNum, int id) {
	NoteModel& note = notes[ch][barNum][id];
	auto& beatAndBar = calcEnd(note.beatNum, note.barNum, note.duration);

	midis[barNum][ch][note.beatNum].erase(note.midi[0].it);
	midis[beatAndBar.second][ch][beatAndBar.first].erase(note.midi[1].it);
	
	notes[ch][barNum].erase(id);
}

std::pair<USHORT, USHORT> ScoreManager::calcEnd(USHORT startBeat, USHORT startBar, USHORT duration) {
	
	USHORT endBeat = startBeat + duration;
	USHORT endBar = startBar;
	if (endBeat >= beat) {
		endBeat -= beat;
		endBar++;
		if (endBar >= barCount) {
			endBar -= barCount;
		}
	}

	return std::make_pair(endBeat, endBar);
}
