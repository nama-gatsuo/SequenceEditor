#pragma once

#include "RhythmMachine.h"
#include "Events.h"
#include "ScoreManager.h"

class Sequencer {
public:
	Sequencer() : globalBeat(0), currentBeat(0), currentBar(0), bpm(120) {}

	void init(ScoreManager& scoreManager, uint32_t bpm) {
		score = &scoreManager;
		setBpm(bpm);

		rhythm.startThread();
		ofAddListener(EventsEntity::newBeat, this, &Sequencer::updateBeat);
	}

	void updateBeat(bool& isNew) {

		globalBeat++;
		currentBar = (globalBeat / score->getBeatCount()) % score->getBarCount();
		currentBeat = globalBeat % score->getBeatCount();
		
		//ofLogNotice() << ofToString(globalBeat);

		if (score != nullptr) {
			score->bang(currentBar, currentBeat);
		}
	
	}
	
	uint8_t getCurrentBeat() const { return currentBeat; }
	uint8_t getCurrentBar() const { return currentBar; }
	uint32_t getBpm() const { return bpm; }
	
	void setBpm(uint8_t bpm) {
		this->bpm = bpm;
		rhythm.setDelta(15000 / bpm);
	}
	int getDelta() { return 15000 / bpm; }

private:
	ScoreManager * score;
	RhythmMachine rhythm;

	uint32_t globalBeat;
	uint8_t currentBeat;
	uint8_t currentBar;
	uint32_t bpm;
};