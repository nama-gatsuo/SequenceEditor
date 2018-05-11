#include "ScoreManager.h"

USHORT ScoreManager::beat = 0;
USHORT ScoreManager::barCount = 0;
USHORT ScoreManager::channelCount = 0;
USHORT ScoreManager::pitchCount = 0;

void ScoreManager::setup(USHORT beat, USHORT barCount, USHORT pitchCount) {
	this->beat = beat;
	this->barCount = barCount;
	this->pitchCount = pitchCount;
	channelCount = 0;
	
	// load from json
	ofJson json = ofLoadJson("json/channelInfo.json");
	
	if (!json.empty()) {
		ofLogNotice("json count: ") << json.size();
		channelCount = json.size();
		
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
		
	} else {
		channelCount = 16;
	}

	midis.assign(this->barCount, vector<BarModel>(channelCount));
	notes.assign(this->barCount, vector<std::unordered_map<int, NoteModel>>(channelCount));

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
		if (!bar[beatNum].empty()) {
			
			auto& info = chanInfos[ch];
			int octave = info.octave + 2;
			int key = static_cast<int>(info.key);
			int si = static_cast<int>(info.scale);
			int ss = ChannelInfo::scaleStep[si].size();

			for (auto& pair : bar[beatNum]) {
				auto& note = pair.second;

				// midi translate
				int yo = (note->y / ss + octave) * 12 + key;
				int yp = ChannelInfo::scaleStep[si][note->y % ss];

				if (note->isAttack) sender.sendMidiOn(ch + 1, yo + yp, note->velocity);
				else sender.sendMidiOff(ch + 1, yo + yp);

			}
			
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

	// y value (pitch) is Key of map
	auto& s = midis[b][c][note.x];
	s.emplace(note.y + pitchCount, std::make_shared<MidiModel>(true, note.velocity, note.y));
	
	auto& beatAndBar = calcEnd(note.x, note.barNum, note.duration);
	auto& e = midis[beatAndBar.second][c][beatAndBar.first];
	e.emplace(note.y, std::make_shared<MidiModel>(false, 0, note.y));
	
	// id is specified with xy-coord in board
	int id = note.x * 16 + note.y;
	notes[b][c].emplace(id, note);
	notes[b][c][id].midiId[0] = note.y + pitchCount;
	notes[b][c][id].midiId[1] = note.y;

	return id;
}

NoteModel& ScoreManager::get(int id) {
	return notes[currentBar][currentChan][id];
}

std::unordered_map<int, NoteModel>& ScoreManager::get() {
	return notes[currentBar][currentChan];
}

int ScoreManager::update(int id, const NoteModel& note) {
	remove(id); // deleted note reference
	return create(note);
}

void ScoreManager::remove(int id) {

	USHORT b = currentBar;
	USHORT c = currentChan;

	NoteModel& n = notes[b][c][id];
	auto& beatAndBar = calcEnd(n.x, n.barNum, n.duration);

	auto& s = midis[b][c][n.x];
	s.erase(n.midiId[0]);

	auto& e = midis[beatAndBar.second][c][beatAndBar.first];
	e.erase(n.midiId[1]);
	
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