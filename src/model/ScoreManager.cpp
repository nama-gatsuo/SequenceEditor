#include "ScoreManager.h"

void ScoreManager::setup(uint8_t beat, uint8_t barCount, uint8_t pitchCount) {
	this->beat = beat;
	this->barCount = barCount;
	this->pitchCount = pitchCount;

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
		ofLogWarning() << "No json found!";
	}

	midis.assign(barCount, std::vector<BarModel>(channelCount));
	notes.assign(barCount, std::vector<std::unordered_map<int, NoteModel>>(channelCount));

}

void ScoreManager::bang(uint8_t barNum, uint8_t beatNum) {

	// check randomize
	if (beatNum == 0) {

		for (auto& chInfo : chanInfos) {
			
			if (chInfo.isRandomLoop) {

				ExecRandom e(barNum, chInfo.channelIndex);
				ofNotifyEvent(EventsEntity::execRandom, e);

			}

		}

	}

	uint8_t ch = 0;
	for (const BarModel& bar : midis[barNum]) {

		if (!bar[beatNum].empty()) {

			auto& info = getChannelInfo(ch);

			for (const auto& pair : bar[beatNum]) {
				auto& note = pair.second;

				// midi translate
				uint8_t midiNote = info.translateMidi(note->y);
				
				if (note->isAttack) {
					if (info.levels[note->level].isActive) {
						sender.sendMidiOn(info.channelIndexInDAW, midiNote, note->velocity);
						//ofLogNotice() << "note: " << (int)midiNote << ", " << "vel: " << note->velocity;
					}
				}
				else sender.sendMidiOff(info.channelIndexInDAW, midiNote);
			}

		}
		ch++;

	}

}

int ScoreManager::create(const NoteModel& note) {

	if (note.x > beat - 1 || note.y > pitchCount - 1) return -1;
	uint8_t c = note.ch;
	uint8_t b = note.barNum;

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


std::unordered_map<int, NoteModel>& ScoreManager::get(uint8_t bar, uint8_t ch) {
	return notes[bar][ch];
}

const NoteModel& ScoreManager::get(uint8_t bar, uint8_t channel, int id) {
	return notes[bar][channel][id];
}

int ScoreManager::update(uint8_t bar, uint8_t channel, int id, const NoteModel& note) {
	remove(bar, channel, id); // deleted note reference
	return create(note);
}

void ScoreManager::remove(uint8_t bar, uint8_t ch, int id) {

	const auto& n = notes[bar][ch][id];
	const auto& beatAndBar = calcEnd(n.x, n.barNum, n.duration);

	auto& s = midis[bar][ch][n.x];
	s.erase(n.midiId[0]);

	auto& e = midis[beatAndBar.second][ch][beatAndBar.first];
	e.erase(n.midiId[1]);

	notes[bar][ch].erase(id);
}

std::pair<uint8_t, uint8_t> ScoreManager::calcEnd(uint8_t startBeat, uint8_t startBar, uint8_t duration) const {

	uint8_t endBeat = startBeat + duration;
	uint8_t endBar = startBar;
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