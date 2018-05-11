#include "UIManager.h"

UIManager::UIManager(int gridSize, int defaultVelocity) : gridSize(gridSize), defaultVelocity(defaultVelocity) {}

UIManager::~UIManager() {
	ofRemoveListener(ofEvents().mouseMoved, this, &UIManager::mouseMoved);
	ofRemoveListener(ofEvents().mousePressed, this, &UIManager::mousePressed);
	ofRemoveListener(ofEvents().mouseDragged, this, &UIManager::mouseDragged);
	ofRemoveListener(ofEvents().mouseReleased, this, &UIManager::mouseReleased);
	ofRemoveListener(ofEvents().keyPressed, this, &UIManager::keyPressed);
	ofRemoveListener(ofEvents().mouseScrolled, this, &UIManager::mouseScrolled);
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
	ofAddListener(ofEvents().mouseScrolled, this, &UIManager::mouseScrolled);

	grids = GridUI(CHANNEL, BAR, PITCH, BEAT);
	grids.setBar(0);
	grids.setChan(0);
	this->score->setCurrent(0, 0);

	state.setup(grids, score);
	gui.setup();
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
	
	// channel info gui
	bool isDraw = true;
	ImGuiWindowFlags window_flags = 0;

	gui.begin();
	ImGui::Begin("Channels", &isDraw, window_flags);
	score->drawChannelInfo();
	ImGui::End();

	ImGui::Begin("Global", &isDraw, window_flags);
	int bar = BAR;
	ImGui::SliderInt("Loop", &bar, 1, 4);
	BAR = bar;

	int bpm = sequencer->getBpm();
	ImGui::SliderInt("BPM", &bpm, 80, 145);
	if (bpm != sequencer->getBpm()) sequencer->setBpm(bpm);
	ImGui::End();
	gui.end();

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

void UIManager::drawGrid() const {
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
	
	// rhythm guide
	USHORT cbeat = sequencer->getCurrentBeat();
	USHORT cbar = sequencer->getCurrentBar();
	USHORT bar = grids.getBar();
	
	if (cbar == bar) {
		int o = 8;
		ofDrawRectangle(cbeat * gridSize + o, PITCH * gridSize + o, gridSize - o * 2, gridSize - o * 2);
	}

	ofSetColor(255);
	for (int i = 0; i < BEAT; i += 4) {
		ofDrawBitmapString(ofToString(bar) + "." + ofToString(i/4), i * gridSize, PITCH * gridSize + 8);
	}

	// draw notes
	auto& pairs = score->get();
	for (auto& pair : pairs) {
		int y = pair.second.y;
		int x = pair.second.x;
		int duration = pair.second.duration;
		int offset = (128.f - pair.second.velocity) / 128.f * gridSize * 0.5;

		ofSetColor(grids.getColor(0));
		ofDrawRectangle(
			offset + x * gridSize, offset + y * gridSize,
			gridSize * duration - offset * 2, gridSize - offset * 2);
	}

	// drag info
	drawStateInfo();

	ofPopStyle();
	ofPopMatrix();
	
}

void UIManager::drawStateInfo() const {

	switch (state.code) {
	case UIState::Code::HOVER_EMPTY: {
		ofSetColor(255, 64);
		ofDrawRectangle(state.current.x * gridSize, state.current.y * gridSize, gridSize, gridSize);
	} break;
	case UIState::Code::DRAG_CREATE: {
		int duration = abs(state.pressed.x - state.current.x) + 1;
		int x = state.pressed.x < state.current.x ? state.pressed.x : state.current.x;
		int y = state.pressed.y;
		int offset = (128.f - defaultVelocity) / 128.f * gridSize * 0.5;

		auto& c = grids.getColor(0);
		ofSetColor(c.r, c.g, c.b, 128);
		ofDrawRectangle(
			offset + x * gridSize, offset + y * gridSize,
			gridSize * duration - offset * 2, gridSize - offset * 2);
	} break;
	case UIState::Code::HOVER_NOTE: {
		auto& n = score->get(state.noteId);
		int d = n.duration;

		ofSetColor(255);
		int x1 = n.x * gridSize;
		int x2 = (0.5 + n.x) * gridSize;
		int y1 = (0.5 + n.y) * gridSize;
		int y2 = (1 + n.y) * gridSize;
		int offset = (128.f - n.velocity) / 128.f * gridSize * 0.5;

		ofNoFill();
		ofDrawRectangle(
			offset + n.x * gridSize, offset + n.y * gridSize,
			gridSize * d - offset * 2, gridSize - offset * 2);

		ofDrawRectangle(x1, y1, 0.5 * gridSize, 0.5 * gridSize);
		ofDrawLine(x1, y1, x2, y2);

		ofDrawBitmapString("velocity: " + ofToString(n.velocity), x2 + 2, y2);
		//ofDrawBitmapString("id: " + ofToString(state.noteId), x2, y1);
	} break;
	case UIState::Code::DRAG_EDIT:
		
		auto& n = score->get(state.noteId);
		int d = state.current.x - n.x + 1;
		if (d < 1) d = 1;

		int offset = (128.f - n.velocity) / 128.f * gridSize * 0.5;
		ofSetColor(255, 200);
		ofNoFill();
		ofDrawRectangle(
			offset + n.x * gridSize, offset + n.y * gridSize,
			gridSize * d - offset * 2, gridSize - offset * 2);
		
		break;
	}

}

void UIManager::mouseMoved(ofMouseEventArgs& args) {
	ivec2 index = translateMousePos(args.x, args.y);
	state.onMouseMoved(index);
}

void UIManager::mousePressed(ofMouseEventArgs& args) {
	state.onMousePressed(isMouseFormer(args.x));
}

void UIManager::mouseDragged(ofMouseEventArgs& args) {
	ivec2 index = translateMousePos(args.x, args.y);
	state.onMouseDragged(index);
}

void UIManager::mouseReleased(ofMouseEventArgs& args) {
	state.onMouseReleased();
}

void UIManager::mouseScrolled(ofMouseEventArgs& args) {
	state.onMouseScrolled(args.scrollY);
}

void UIManager::keyPressed(ofKeyEventArgs& args) {

	switch (args.key) {
	case OF_KEY_UP: {
		int ch = grids.getChan() + 1;
		if (ch >= CHANNEL) ch -= CHANNEL;
		else if (ch < 0) ch += CHANNEL;

		grids.setChan(ch);
		score->setChan(ch);
	} break;
	case OF_KEY_DOWN: {
		int ch = grids.getChan() - 1;
		if (ch >= CHANNEL) ch -= CHANNEL;
		else if (ch < 0) ch += CHANNEL;

		grids.setChan(ch);
		score->setChan(ch);
	} break;
	case OF_KEY_RIGHT: {
		int bar = grids.getBar() + 1;
		if (bar >= BAR) bar -= BAR;
		else if (bar < 0) bar += BAR;

		grids.setBar(bar);
		score->setBar(bar);
	} break;
	case OF_KEY_LEFT: {
		int bar = grids.getBar() - 1;
		if (bar >= BAR) bar -= BAR;
		else if (bar < 0) bar += BAR;

		grids.setBar(bar);
		score->setBar(bar);
	} break;
	default:
		break;
	}

}

bool UIManager::isMouseFormer(int x) const {
	x -= startPos.x;
	return (x / (int)(gridSize * 0.5)) % 2 == 0;
}

