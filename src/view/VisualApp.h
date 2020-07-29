#pragma once
#include "ofMain.h"
#include "ScoreManager.h"
#include "VisBoxes.h"
#include "ofxDeferredShading.h"
#include "ofxDeferredHelper.h"

class VisualApp : public ofBaseApp {
public:
	void setup() {
		ofSetFrameRate(30);
		ofSetVerticalSync(true);
		ofSetWindowTitle("visual");
		ofEnableDepthTest();
		//ofToggleFullscreen();

		vb.setup();
		
		helper.init(1920, 1080);
		
		isDebug = true;
	};

	void update() {
		cam.setPosition(800 * cos(ofGetElapsedTimef() * 0.2), 100 * cos(ofGetElapsedTimef() * 0.1), 800 * sin(ofGetElapsedTimef() * 0.2));
		cam.lookAt(glm::vec3(0, 0, 0));

		vb.update(*score, *sequencer);

	};
	void draw() {

		helper.render([&](const ofxDeferred::RenderInfo& info) {
			vb.draw(info.lds, info.isShadow);
		}, cam);

		if (isDebug) {
			helper.debugDraw();
			helper.drawGui();
		}
		
		ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 20, 20);
	};
	
	void keyPressed(int key) {
		if (key == 'd') {
			isDebug = !isDebug;
		} else if (key == 's') {
			helper.saveParams();
		}
	}

	ScoreManager* score;
	Sequencer* sequencer;

private:
	//UIManager* ui;
	ofEasyCam cam;
	VisBox vb;
	
	ofxDeferred::Helper helper;
	bool isDebug;

};