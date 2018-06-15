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
	};
	void update() {
		//cam.setPosition(500 * cos(ofGetElapsedTimef()), 0, sin(ofGetElapsedTimef()));
		//cam.lookAt(vec3(0, 0, 0));
	};
	void draw() {
		
		cam.begin();

		auto& m = score->getMidis();
		
		int barNum = 0;
		for (auto& bm : m) {
			
			int chNum = 0;
			for (auto& bar : bm) {

				float x = ofMap(chNum, 0, 16, -80, 80);

				for (int i = 0; i < 16; i++) {
					auto& map = bar[i];

					float z = ofMap(i, 0, 16, 0, 160);
					z += barNum * 180;

					for (auto& midi : map) {

						if (midi.second->isAttack) {
							ofSetColor(255, 128, 128);
						} else {
							ofSetColor(128, 128, 255);
						}

						float size = ofMap(midi.second->velocity, 0, 128, 5, 10);
						float y = ofMap(midi.second->y, 0, 128, -5*64, 5*64);

						ofDrawBox(vec3(x, y, z), size);


					}

				}
				chNum++;
			}
			barNum++;
		}
		
		cam.end();
		 
		//ofDrawCircle(vec2(ofGetWidth() * (0.5 + sin(ofGetElapsedTimef()) * 0.5), 540), 100);
		ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 20, 20);
	};

	ofEasyCam cam;

	ScoreManager* score;
	//UIManager* ui;

};