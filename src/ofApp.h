#pragma once

#include "ofMain.h"
#include "ScoreManager.h"
#include "Sequencer.h"
#include "UIManager.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	ScoreManager score;
	Sequencer sequencer;
	UIManager ui;
};
