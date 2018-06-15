#pragma once
#include "ofMain.h"
#include "ScoreManager.h"

using namespace glm;

class VisualApp : public ofBaseApp {
public:
	void setup() {
		ofSetFrameRate(60);
		ofSetVerticalSync(true);
		ofSetWindowTitle("visual");
		ofEnableDepthTest();
	};
	void update() {
		//cam.setPosition(500 * cos(ofGetElapsedTimef()), 0, sin(ofGetElapsedTimef()));
		//cam.lookAt(vec3(0, 0, 0));
	};
	void draw() {
		
		cam.begin();

		auto& m = score->getNotes();
		
		int barNum = 0;
		
		
		cam.end();
		 
		//ofDrawCircle(vec2(ofGetWidth() * (0.5 + sin(ofGetElapsedTimef()) * 0.5), 540), 100);
		ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 20, 20);
	};

	ofEasyCam cam;

	ScoreManager* score;
	//UIManager* ui;

};