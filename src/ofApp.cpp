#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofBackground(0);

	// 16 beat, 4 bar loop, 16 instrument(midi ch)
	score.setup(16, 4, 16);
	
	// bpm = 120
	sequenser.init(score, 120);


}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {
	int h = 0;

	ofDrawBitmapString("bpm: " + ofToString(sequenser.getBpm()), 16, 32 + 16 * (++h));
	ofDrawBitmapString("bar: " + ofToString(sequenser.getCurrentBar()), 16, 32 + 16 * (++h));
	ofDrawBitmapString("beat: " + ofToString(sequenser.getCurrentBeat()), 16, 32 + 16 * (++h));
	ofDrawBitmapString("delta: " + ofToString(sequenser.getDelta()), 16, 32 + 16 * (++h));
	h++;
	ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 16, 32 + 16 * (++h));
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

