#include "ScoreManager.h"

USHORT ScoreManager::beat = 0;
USHORT ScoreManager::barCount = 0;
USHORT ScoreManager::channelCount = 0;
USHORT ScoreManager::pitchCount = 0;

void ScoreManager::setup(USHORT beat, USHORT barCount, USHORT pitchCount) {
	this->beat = beat;
	this->barCount = barCount;
	this->pitchCount = pitchCount;
	this->channelCount = 16;

	midis.assign(this->barCount, vector<BarModel>(this->channelCount));
	notes.assign(this->barCount, vector<std::unordered_map<int, NoteModel>>(this->channelCount));
	
	// load from json
	ofJson json = ofLoadJson("json/channelInfo.json");
	if (!json.empty()) {
		ofLogNotice("json count: ") << json.size();
		//this->channelCount = (USHORT)json.size();
		
		int i = 0;
		for (auto& data : json) {
			string name = data["name"];
			ChannelInfo ch(i, name);
			
			ch.octave = data["octave"];
			ch.key = static_cast<ChannelInfo::Key>(data["key"]);
			ch.scale = static_cast<ChannelInfo::Scale>(data["scale"]);
			
			chanInfos.push_back(ch);
			
			i++;
		}

	}

	// create scale
	ChannelInfo::scaleStep.assign(5, vector<int>());
	ChannelInfo::scaleStep[0] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	ChannelInfo::scaleStep[1] = { 0, 2, 4, 5, 7, 9, 11 };
	ChannelInfo::scaleStep[2] = { 0, 2, 3, 5, 7, 8, 10 };
	ChannelInfo::scaleStep[3] = { 0, 2, 4, 7, 9 };
}

void ScoreManager::bang(USHORT barNum, USHORT beatNum) {

	USHORT ch = 0;
	for (BarModel& bar : midis[barNum]) {
		
		auto& info = chanInfos[ch];
		
		for (MidiModel::Ptr& note : bar[beatNum]) {

			// midi translate
			int octave = info.octave + 2;
			int key = static_cast<int>(info.key);
			int si = static_cast<int>(info.scale);

			int ss = ChannelInfo::scaleStep[si].size();
			int yo = (note->pitch / ss + octave) * 12 + key;
			
			int yp = ChannelInfo::scaleStep[si][note->pitch % ss];

			if (note->isAttack) sender.sendMidiOn(ch+1, yo + yp, note->velocity);
			else sender.sendMidiOff(ch+1, yo + yp);

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

void ScoreManager::drawChannelInfo() {
	for (auto& info : chanInfos) {
		info.drawGui();
	}
	
}