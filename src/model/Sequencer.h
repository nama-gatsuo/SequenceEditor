#pragma once
#include "ofMain.h"
#include "RhythmMachine.h"
#include "Events.h"
#include "ScoreManager.h"

class Sequencer {
public:
	Sequencer() : globalBeat(0), currentBeat(0), currentBar(0), bpm(120) {}

	void init(ScoreManager& scoreManager, UINT bpm) {
		score = &scoreManager;
		setBpm(bpm);

		rhythm.startThread();
		ofAddListener(EventsEntity::newBeat, this, &Sequencer::updateBeat);
	}

	void updateBeat(bool& isNew) {

		globalBeat++;
		currentBar = (globalBeat / score->getBeat()) % score->getBarCount();
		currentBeat = globalBeat % score->getBeat();
		
		//ofLogNotice() << ofToString(globalBeat);

		if (score != nullptr) {
			score->bang(currentBar, currentBeat);
		}
	
	}
	
	unsigned char getCurrentBeat() const { return currentBeat; }
	unsigned char getCurrentBar() const { return currentBar; }
	UINT getBpm() const { return bpm; }
	
	void setBpm(UINT bpm) {
		this->bpm = bpm;
		rhythm.setDelta(15000 / bpm);
	}
	int getDelta() { return 15000 / bpm; }

private:
	ScoreManager * score;
	RhythmMachine rhythm;

	ULONG globalBeat;
	unsigned char currentBeat;
	unsigned char currentBar;
	UINT bpm;
};