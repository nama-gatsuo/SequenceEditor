#include "ScoreManager.h"

UCHAR ScoreManager::beat = 0;
UCHAR ScoreManager::barCount = 0;
UCHAR ScoreManager::channelCount = 0;
UCHAR ScoreManager::pitchCount = 0;

void ScoreManager::setup(UCHAR beat, UCHAR barCount, UCHAR pitchCount) {
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
			float hue = ofToFloat(data["hue"]);

			ChannelInfo ch(i, name, hue);

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

void ScoreManager::bang(UCHAR barNum, UCHAR beatNum) {

	UCHAR ch = 0;
	for (BarModel& bar : midis[barNum]) {
		if (!bar[beatNum].empty()) {

			auto& info = getChannelInfo(ch);

			for (auto& pair : bar[beatNum]) {
				auto& note = pair.second;

				// midi translate
				UCHAR midiNote = info.translateMidi(note->y);
				
				if (note->isAttack) {
					if (info.isActive[note->level]) {
						sender.sendMidiOn(ch + 1, midiNote, note->velocity);
					}
				}
				else sender.sendMidiOff(ch + 1, midiNote);
			}

		}
		ch++;

	}

}

void ScoreManager::setCurrent(UCHAR bar, UCHAR ch) {
	currentBar = bar;
	currentChan = ch;
}

int ScoreManager::create(const NoteModel& note) {

	UCHAR b = currentBar;
	UCHAR c = currentChan;

	// y value (pitch) is Key of map
	auto& s = midis[b][c][note.x];
	s.emplace(note.y + pitchCount, std::make_shared<MidiModel>(true, note.velocity, note.y, note.level));

	auto& beatAndBar = calcEnd(note.x, note.barNum, note.duration);
	auto& e = midis[beatAndBar.second][c][beatAndBar.first];
	e.emplace(note.y, std::make_shared<MidiModel>(false, 0, note.y, note.level));

	// id is specified with xy-coord in board
	int id = note.x * 16 + note.y;
	notes[b][c].emplace(id, note);
	notes[b][c][id].midiId[0] = note.y + pitchCount;
	notes[b][c][id].midiId[1] = note.y;

	/*
	cout << "-- create --" << endl;
	cout << "bar:" + ofToString((int)b) << endl;
	cout << "ch:" + ofToString((int)c) << endl;
	cout << "x:" + ofToString((int)note.x) << endl;
	cout << "y:" + ofToString((int)note.y) << endl;
	cout << "duration:" + ofToString((int)note.duration) << endl;
	cout << "id:" + ofToString(id) << endl;
	cout << "level:" + ofToString((int)note.level) << endl;
	*/

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

	UCHAR b = currentBar;
	UCHAR c = currentChan;

	NoteModel& n = notes[b][c][id];
	auto& beatAndBar = calcEnd(n.x, n.barNum, n.duration);

	auto& s = midis[b][c][n.x];
	s.erase(n.midiId[0]);

	auto& e = midis[beatAndBar.second][c][beatAndBar.first];
	e.erase(n.midiId[1]);

	notes[b][c].erase(id);
}

//void ScoreManager::clearCurrent(UCHAR level) {
//	UCHAR c = currentChan;
//	UCHAR b = currentBar;
//
//	
//
//}

std::pair<UCHAR, UCHAR> ScoreManager::calcEnd(UCHAR startBeat, UCHAR startBar, UCHAR duration) const {

	UCHAR endBeat = startBeat + duration;
	UCHAR endBar = startBar;
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