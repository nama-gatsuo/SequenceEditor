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

void ScoreManager::setCurrent(USHORT bar, USHORT ch) {
	currentBar = bar;
	currentChan = ch;
}

int ScoreManager::create(const NoteModel& note) {
	
	USHORT b = currentBar;
	USHORT c = currentChan;

	auto& s = midis[b][c][note.beatNum];
	s.push_back(std::make_shared<MidiModel>(true, note.velocity, note.pitch));
	
	auto& beatAndBar = calcEnd(note.beatNum, note.barNum, note.duration);
	auto& e = midis[beatAndBar.second][c][beatAndBar.first];
	e.push_back(std::make_shared<MidiModel>(false, 0, note.pitch));
	
	int lastId = notes[b][c].size();
	notes[b][c].emplace(lastId, note);
	notes[b][c][lastId].midi[0].it = s.cbegin() + s.size() - 1;
	notes[b][c][lastId].midi[1].it = e.cbegin() + e.size() - 1;

	return lastId;
}

NoteModel& ScoreManager::get(int id) {
	return notes[currentBar][currentChan][id];
}

std::unordered_map<int, NoteModel>& ScoreManager::get() {
	return notes[currentBar][currentChan];
}

int ScoreManager::update(int id, const NoteModel& note) {
	remove(id);
	return create(note);
}

void ScoreManager::remove(int id) {

	USHORT b = currentBar;
	USHORT c = currentChan;

	NoteModel& note = notes[b][c][id];
	auto& beatAndBar = calcEnd(note.beatNum, note.barNum, note.duration);

	midis[b][c][note.beatNum].erase(note.midi[0].it);
	midis[beatAndBar.second][c][beatAndBar.first].erase(note.midi[1].it);
	
	notes[b][c].erase(id);
}

std::pair<USHORT, USHORT> ScoreManager::calcEnd(USHORT startBeat, USHORT startBar, USHORT duration) const {
	
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
