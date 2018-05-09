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

	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

private:
	ScoreManager score;
	Sequencer sequencer;
	UIManager ui;
};
