#include "UIManager.h"

UIManager::UIManager(int gridSize, int defaultVelocity) : gridSize(gridSize), defaultVelocity(defaultVelocity) {}

UIManager::~UIManager() {
	ofRemoveListener(ofEvents().mouseMoved, this, &UIManager::mouseMoved);
	ofRemoveListener(ofEvents().mousePressed, this, &UIManager::mousePressed);
	ofRemoveListener(ofEvents().mouseDragged, this, &UIManager::mouseDragged);
	ofRemoveListener(ofEvents().mouseReleased, this, &UIManager::mouseReleased);
	ofRemoveListener(ofEvents().keyPressed, this, &UIManager::keyPressed);
}

void UIManager::setup(ScoreManager& score, Sequencer& sequencer) {
	this->score = &score;
	this->sequencer = &sequencer;

	size = ivec2(gridSize * BEAT, gridSize * PITCH);

	ofAddListener(ofEvents().mouseMoved, this, &UIManager::mouseMoved);
	ofAddListener(ofEvents().mousePressed, this, &UIManager::mousePressed);
	ofAddListener(ofEvents().mouseDragged, this, &UIManager::mouseDragged);
	ofAddListener(ofEvents().mouseReleased, this, &UIManager::mouseReleased);
	ofAddListener(ofEvents().keyPressed, this, &UIManager::keyPressed);

	grids = GridUI(CHANNEL, BAR, PITCH, BEAT);
	grids.setBar(0);
	grids.setChan(0);

	state.code = State::Code::FREE;
}

void UIManager::draw(int offsetX, int offsetY) {
	
	// text info
	int w = 160;
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

	h = 0;
	ofDrawBitmapString("bar: " + ofToString(grids.getBar()), offsetX + w, offsetY + lh * (++h));
	ofDrawBitmapString("channel: " + ofToString(grids.getChan()), offsetX + w, offsetY + lh * (++h));
	ofDrawBitmapString("state: " + ofToString((int)state.code), offsetX + w, offsetY + lh * (++h));

	// grid
	drawGrid();
	
	// mouse
	if (state.code != State::Code::FREE) {
		ofDrawBitmapString("(" + ofToString(state.current.x) + "," + ofToString(state.current.y) + ")", ofGetMouseX(), ofGetMouseY());
	}
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
	ofPushStyle();

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
	auto& pairs = score->get(grids.getChan(), grids.getBar());
	for (auto& pair : pairs) {
		int y = pair.second.pitch;
		int x = pair.second.beatNum;
		int duration = pair.second.duration;
		int offset = (128.f - pair.second.velocity) / 128.f * gridSize * 0.5;

		ofSetColor(grids.getColor(0));
		ofDrawRectangle(
			offset + x * gridSize, offset + y * gridSize,
			gridSize * duration - offset * 2, gridSize - offset * 2);
	}

	// drag info
	if (state.code == State::Code::DRAG_CREATE) {
		
		int duration = abs(state.pressed.x - state.current.x) + 1;
		int x = state.pressed.x < state.current.x ? state.pressed.x : state.current.x;
		int y = state.pressed.y;
		int offset = (128.f - defaultVelocity) / 128.f * gridSize * 0.5;

		ofColor& c = grids.getColor(0);
		ofSetColor(c.r, c.g, c.b, 128);
		ofDrawRectangle(
			offset + x * gridSize, offset + y * gridSize,
			gridSize * duration - offset * 2, gridSize - offset * 2);
	} else if (state.code == State::Code::HOVER_NOTE) {
		auto& pair = score->get(grids.getChan(), grids.getBar(), state.noteId);
		int y = pair.pitch;
		int x = pair.beatNum;
		int d = pair.duration;
		

		ofSetColor(255);
		int x1 = x * gridSize;
		int x2 = (0.5 + x) * gridSize;
		int y1 = (0.5 + y) * gridSize;
		int y2 = (1 + y) * gridSize;

		ofDrawLine(x1, y1, x1, y2);
		ofDrawLine(x1, y1, x2, y2);
		ofDrawLine(x1, y1, x2, y1);
		ofDrawLine(x1, y2, x2, y1);
		ofDrawLine(x1, y2, x2, y2);
		ofDrawLine(x2, y1, x2, y2);

		x1 = (x + d - 0.25) * gridSize;
		x2 = (x + d) * gridSize;
		y1 = y * gridSize;
		y2 = (y + 1) * gridSize;
		ofDrawLine(x1, y1, x1, y2);
		ofDrawLine(x1, y1, x2, y1);
		ofDrawLine(x1, y2, x2, y2);
		ofDrawLine(x2, y1, x2, y2);
	}

	ofPopStyle();
	ofPopMatrix();
	
}

void UIManager::mouseMoved(ofMouseEventArgs& args) {

	state.current = translateMousePos(args.x, args.y);
	bool isHoverOnGrid = !any(lessThan(state.current, ivec2(0)));

	switch (state.code) {
	case State::Code::FREE:
		if (isHoverOnGrid) {	
			state.noteId = grids.get()[state.current.x][state.current.y];
			if (state.noteId < 0) {
				state.code = State::Code::HOVER_EMPTY;
			} else {
				state.code = State::Code::HOVER_NOTE;
			}
		}
		break;
	case State::Code::HOVER_EMPTY:
		if (isHoverOnGrid) {
			state.noteId = grids.get()[state.current.x][state.current.y];
			if (state.noteId != -1) {
				state.code = State::Code::HOVER_NOTE;
			}
		} else {
			state.code = State::Code::FREE;
		}
		break;
	case State::Code::HOVER_NOTE:
		if (isHoverOnGrid) {
			state.noteId = grids.get()[state.current.x][state.current.y];
			if (state.noteId == -1) {
				state.code = State::Code::HOVER_EMPTY;
			}
		} else {
			state.code = State::Code::FREE;
		}
		break;
	default: break;
	}

}

void UIManager::mousePressed(ofMouseEventArgs& args) {

	state.pressed = state.current;

	switch (state.code) {
	case State::Code::HOVER_EMPTY:
		state.code = State::Code::DRAG_CREATE;
		break;
	case State::Code::HOVER_NOTE:
		state.code = State::Code::DRAG_EDIT;
		break;
	default: break;
	}

}

void UIManager::mouseDragged(ofMouseEventArgs& args) {
	
	ivec2 index = translateMousePos(args.x, args.y);
	bool isHoverOnGrid = !any(lessThan(index, ivec2(0)));

	switch (state.code) {
	case State::Code::DRAG_CREATE:
		if (isHoverOnGrid) {
			state.current = index;
		}
		break;
	case State::Code::DRAG_EDIT:
		if (isHoverOnGrid) {
			state.current = index;
		}
		break;
	default: break;
	}

}

void UIManager::mouseReleased(ofMouseEventArgs& args) {
	switch (state.code) {
	case State::Code::DRAG_CREATE: {
		// create
		int duration = abs(state.pressed.x - state.current.x) + 1;
		int x = state.pressed.x < state.current.x ? state.pressed.x : state.current.x;
		int y = state.pressed.y;

		NoteModel n(grids.getChan(), grids.getBar(), x, y, defaultVelocity, duration);
		int id = score->create(grids.getChan(), grids.getBar(), n);
		for (int i = 0; i < duration; i++) {
			grids.get()[x + i][y] = id;
		}

		// reset
		state.code = State::Code::HOVER_NOTE;
		state.pressed = ivec2(-1);
		state.noteId = id;
	} break;
	case State::Code::DRAG_EDIT:
		// edit
		

		// reset
		state.code = State::Code::HOVER_NOTE;
		state.pressed = ivec2(-1);
		break;
	default: break;
	}

}

void UIManager::keyPressed(ofKeyEventArgs& args) {
	switch (args.key) {
	case OF_KEY_UP:
		grids.setChan((grids.getChan()) + 1);
		break;
	case OF_KEY_DOWN:
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