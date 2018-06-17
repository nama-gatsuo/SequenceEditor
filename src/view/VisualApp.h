#pragma once
#include "ofMain.h"
#include "ScoreManager.h"
#include "VisBoxes.h"
#include "ofxDeferredShading.h"
#include "ofxDeferredParams.h"

using namespace glm;
using namespace ofxDeferred;

class VisualApp : public ofBaseApp {
public:
	void setup() {
		ofSetFrameRate(30);
		ofSetVerticalSync(true);
		ofSetWindowTitle("visual");
		ofEnableDepthTest();
		ofToggleFullscreen();

		vb.setup();
		
		deferred.init(1920, 1080);
		params.setup(deferred);
		

	};

	void update() {
		cam.setPosition(800 * cos(ofGetElapsedTimef() * 0.2), 100 * cos(ofGetElapsedTimef() * 0.1), 3200 * sin(ofGetElapsedTimef() * 0.2));
		cam.lookAt(vec3(0, 0, 0));

		vb.update(*score, *sequencer);

	};
	void draw() {

		params.getShadow()->beginShadowMap(true);
		vb.draw(params.getShadow()->getCamera(), true);
		params.getPointLight()->drawLights(params.getShadow()->getCamera(), true);
		params.getShadow()->endShadowMap();

		deferred.begin(cam);
		vb.draw(cam, false);
		params.getPointLight()->drawLights(cam, false);
		deferred.end();

		params.drawGui();
		ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 20, 20);
	};

	ScoreManager* score;
	Sequencer* sequencer;

private:
	//UIManager* ui;
	ofEasyCam cam;
	VisBox vb;

	ofxDeferredProcessing deferred;
	
	ofxDeferredParams params;
};