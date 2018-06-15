#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofSetWindowTitle("sequencer");
	ofBackground(0);

	// 16 beats, 4 bars loop, 16 instruments (midi channels)
	score.setup(16, 4, 16);
	
	// bpm = 120
	sequencer.init(score, 120);
	
	ui.setup(score, sequencer);
	ui.setPosition(64, ofGetHeight() - 64);

}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {
	
	ui.draw(32, 32);

}