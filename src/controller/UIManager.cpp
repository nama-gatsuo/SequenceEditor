#include "UIManager.h"

UIManager::UIManager(int gridSize) : gridSize(gridSize) {}

UIManager::~UIManager() {
	ofRemoveListener(ofEvents().mouseMoved, this, &UIManager::mouseMoved);
	ofRemoveListener(ofEvents().keyPressed, this, &UIManager::keyPressed);
}

void UIManager::setup(ScoreManager& score, Sequencer& sequencer) {
	this->score = &score;
	this->sequencer = &sequencer;

	size = ivec2(gridSize * BEAT, gridSize * PITCH);

	ofAddListener(ofEvents().mouseMoved, this, &UIManager::mouseMoved);
	ofAddListener(ofEvents().keyPressed, this, &UIManager::keyPressed);

	grids = GridUI(CHANNEL, BAR, PITCH, BEAT);
	grids.setBar(0);
	grids.setChan(0);
}

void UIManager::draw(int offsetX, int offsetY) {
	int h = 0;
	int lh = 16; // line height

	ofSetColor(255);
	ofSetLineWidth(0.1);

	ofDrawBitmapString("bpm: " + ofToString(sequencer->getBpm()), offsetX, offsetY + lh * (++h));
	ofDrawBitmapString("bar: " + ofToString(sequencer->getCurrentBar()), offsetX, offsetY + lh * (++h));
	ofDrawBitmapString("beat: " + ofToString(sequencer->getCurrentBeat()), offsetX, offsetY + lh * (++h));
	ofDrawBitmapString("delta: " + ofToString(sequencer->getDelta()), offsetX, offsetY + lh * (++h));
	h++;
	ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), offsetX, offsetY + lh * (++h));

	if (hover.isGridHover) {
		ofDrawBitmapString("(" + ofToString(hover.gridIndex.x) + "," + ofToString(hover.gridIndex.y) + ")", ofGetMouseX(), ofGetMouseY());
	}


	// grid
	drawGrid();
}

ivec2 UIManager::translateMousePos(int x, int y) {
	int beginX = startPos.x;
	int beginY = startPos.y - size.y;
	int endX = startPos.x + size.x;
	int endY = startPos.y;

	if ((x > beginX && x < endX) && (y > beginY && y < endY)) {
		x -= beginX;
		y -= beginY;
		x /= gridSize;
		y /= gridSize;
		y = PITCH - y - 1;

		return ivec2(x, y);
	} else {
		return ivec2(-1);
	}

}

void UIManager::drawGrid() {
	ofPushMatrix();
	ofTranslate(startPos.x, startPos.y);
	ofScale(1, -1);
	
	// base grid
	ofSetColor(128);
	for (int x = 0; x <= size.x; x += gridSize) {
		ofDrawLine(x, 0, x, size.y);
	}
	for (int y = 0; y <= size.y; y += gridSize) {
		ofDrawLine(0, y, size.x, y);
	}
	
	// draw notes


	ofPopMatrix();
	
}

void UIManager::mouseMoved(ofMouseEventArgs& args) {

	ivec2 index = translateMousePos(args.x, args.y);
	hover.isGridHover = !any(lessThan(index, ivec2(0)));
	hover.gridIndex = index;

}

void UIManager::keyPressed(ofKeyEventArgs& args) {
	switch (args.key) {
		case OF_KEY_DOWN:
			grids.setChan((grids.getChan()) + 1);
			break;
		case OF_KEY_UP:
			grids.setChan((grids.getChan()) - 1);
			break;
		case OF_KEY_RIGHT:
			grids.setBar((grids.getBar()) + 1);
			break;
		case OF_KEY_LEFT:
			grids.setBar((grids.getBar()) - 1);
			break;
		default:
			break;
	}


}