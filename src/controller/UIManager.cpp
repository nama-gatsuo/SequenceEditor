#include "UIManager.h"

UIManager::UIManager(int gridSize) : gridSize(gridSize) {}

UIManager::~UIManager() {
	ofRemoveListener(ofEvents().mouseMoved, this, &UIManager::mouseMoved);
	ofRemoveListener(ofEvents().mousePressed, this, &UIManager::mousePressed);
	ofRemoveListener(ofEvents().mouseDragged, this, &UIManager::mouseDragged);
	ofRemoveListener(ofEvents().mouseReleased, this, &UIManager::mouseReleased);
	ofRemoveListener(ofEvents().keyPressed, this, &UIManager::keyPressed);
	ofRemoveListener(ofEvents().mouseScrolled, this, &UIManager::mouseScrolled);

	ofRemoveListener(EventsEntity::execRandom, this, &UIManager::randomize);
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

	ofAddListener(EventsEntity::execRandom, this, &UIManager::randomize);

	grids = GridUI(CHANNEL, BAR, PITCH, BEAT);
	grids.setBar(0);
	grids.setChan(0, this->score->getChannelInfo(0).hue);
	this->score->setCurrent(0, 0);

	state.setup(grids, score);
	gui.setup();

	font.load("font/Track.ttf", 600, true);
	font.setLetterSpacing(1.037);
}

void UIManager::draw(int offsetX, int offsetY) {
	
	// text info
	ofSetColor(grids.getColor(2) * 0.3);
	font.drawString(ofToString((int)grids.getBar()), startPos.x + 60, startPos.y);
	ofSetColor(255);
	ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 15, 15);

	// grid
	drawGrid();
	
	// ImGui
	bool isDraw = true;
	ImGuiWindowFlags window_flags = 0;

	gui.begin();
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImColor::ImColor(0.));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImColor::ImColor(0.));
	
	// channel select
	ImGui::Begin("ChannelSelect", &isDraw, window_flags);
	int currentCh = grids.getChan();
	ImGui::PushID("chselect");
	for (int i = 0; i < 16; i++) {
		auto& col1 = score->getChannelInfo(i).colors[0];
		auto& col2 = score->getChannelInfo(i).colors[1];
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, col1);
		ImGui::PushStyleColor(ImGuiCol_Header, col2);
		
		bool b = currentCh == i;
		if (ImGui::Selectable(ofToString(i).data(), &b, 0, ImVec2(30, 30))) {
			grids.setChan(i, score->getChannelInfo(i).hue);
			score->setChan(i);
		}

		if (i % 8 != 7) ImGui::SameLine();
		ImGui::PopStyleColor(2);
	}
	ImGui::PopID();
	ImGui::End();
	
	
	// channel info
	ImGui::Begin("Channels", &isDraw, window_flags);
	score->getChannelInfo().drawGui();
	ImGui::End();

	// global info
	ImGui::Begin("Global", &isDraw, window_flags);
	int bar = BAR;
	ImGui::SliderInt("Loop", &bar, 1, 4);
	BAR = bar;

	int bpm = sequencer->getBpm();
	ImGui::SliderInt("BPM", &bpm, 80, 145);
	if (bpm != sequencer->getBpm()) sequencer->setBpm(bpm);

	ImGui::SliderInt("Velocity", &state.defaultVelocity, 1, 128);

	ImGui::PushID("edit");
	ImGui::Text("edit  :"); ImGui::SameLine();
	for (int i = 0; i < 4; i++) {
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, score->getChannelInfo().colors[i]);
		ImGui::PushStyleColor(ImGuiCol_Header, score->getChannelInfo().colors[i]);
		bool b = state.currentEditLevel == i;
		if (ImGui::Selectable(ofToString(i).data(), &b, 0, ImVec2(30, 30))) {
			state.currentEditLevel = i;
		}
		
		if (i != 3) ImGui::SameLine();
		ImGui::PopStyleColor(2);
	}
	ImGui::PopID();

	ImGui::PushID("clear");
	ImGui::Text("clear :"); ImGui::SameLine();
	for (int i = 0; i < 4; i++) {
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, score->getChannelInfo().colors[i]);
		ImGui::PushStyleColor(ImGuiCol_Header, score->getChannelInfo().colors[i]);
		bool b = false;
		if (ImGui::Selectable(ofToString(i).data(), &b, 0, ImVec2(30, 30))) {
			clear(grids.getBar(), grids.getChan(), i);
		}

		if (i != 3) ImGui::SameLine();
		ImGui::PopStyleColor(2);
	}
	ImGui::PopID();

	ImGui::End();
	
	ImGui::PopStyleColor(2);
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
	ofSetColor(255);
	for (int x = 0; x <= size.x; x += gridSize * 4) {
		ofDrawLine(x, 0, x, size.y);
	}
	for (int y = 0; y <= size.y; y += gridSize * 4) {
		ofDrawLine(0, y, size.x, y);
	}

	
	// rhythm guide
	UCHAR cbeat = sequencer->getCurrentBeat();
	UCHAR cbar = sequencer->getCurrentBar();
	UCHAR bar = grids.getBar();
	
	if (cbar == bar) {
		int o = 4;
		ofNoFill();
		ofDrawRectangle(cbeat * gridSize + o, o, gridSize - o * 2, gridSize * (PITCH + 1) - o * 2);
		
		o = 8;
		ofFill();
		ofDrawRectangle(cbeat * gridSize + o, PITCH * gridSize + o, gridSize - o * 2, gridSize - o * 2);
	}

	ofSetColor(255);
	for (int i = 0; i < BEAT; i += 4) {
		ofDrawBitmapString(ofToString((int)bar) + "." + ofToString(i/4), i * gridSize, PITCH * gridSize + 8);
	}

	// pitch guide
	for (int i = 0; i < CHANNEL; i++) {
		int midiPitch = score->getChannelInfo().translateMidi(i);
		ofDrawBitmapString(ofToString(midiPitch), - 20, gridSize * i + 12);
	}

	// draw notes
	auto& pairs = score->get();
	ofPushStyle();
	for (auto& pair : pairs) {
		UCHAR y = pair.second.y;
		UCHAR x = pair.second.x;
		UCHAR level = pair.second.level;

		if (y > 15 || x > 15 || level > 3) continue;

		ChannelInfo ci = score->getChannelInfo();
		bool isActive = ci.isActive[level];

		int duration = pair.second.duration;
		int offset = (128.f - pair.second.velocity) / 128.f * gridSize * 0.5;

		ofSetColor(grids.getColor(level));

		if (isActive) ofFill();
		else ofNoFill();

		ofDrawRectangle(
			offset + (int)x * gridSize, offset + (int)y * gridSize,
			gridSize * duration - offset * 2, gridSize - offset * 2);
		

	}
	ofPopStyle();

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
		int offset = (128.f - state.defaultVelocity) / 128.f * gridSize * 0.5;

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

		ofDrawBitmapString("velocity: " + ofToString((int)n.velocity), x2 + 2, y2);
		ofDrawBitmapString("id: " + ofToString(state.noteId), x2, y1);
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

		grids.setChan(ch, score->getChannelInfo(ch).hue);
		score->setChan(ch);
	} break;
	case OF_KEY_DOWN: {
		int ch = grids.getChan() - 1;
		if (ch >= CHANNEL) ch -= CHANNEL;
		else if (ch < 0) ch += CHANNEL;

		grids.setChan(ch, score->getChannelInfo(ch).hue);
		score->setChan(ch);
	} break;
	case OF_KEY_RIGHT: {
		int bar = grids.getBar() + 1;
		if (bar >= MAX_BAR) bar -= MAX_BAR;
		else if (bar < 0) bar += MAX_BAR;

		grids.setBar(bar);
		score->setBar(bar);
	} break;
	case OF_KEY_LEFT: {
		int bar = grids.getBar() - 1;
		if (bar >= MAX_BAR) bar -= MAX_BAR;
		else if (bar < 0) bar += MAX_BAR;

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

void UIManager::clear(UCHAR bar, UCHAR ch, UCHAR level) {
	
	auto& g = grids.get(bar, ch);
	
	for (UCHAR x = 0; x < BEAT; x++) {
		for (UCHAR y = 0; y < PITCH; y++) {
			UCHAR id = g[x][y][0];
			UCHAR lev = g[x][y][1];

			if (lev == level) {
				g[x][y] = { -1, -1 };
				UCHAR d = score->get(bar, ch)[id].duration;

				if (d > 15 || y > 15 || x > 15) continue;

				for (UCHAR i = 0; i < d; i++) {
					g[x+i][y] = { -1, -1 };
				}

				score->remove(bar, ch, id);

			}

		}
	
	}

}

void UIManager::randomize(ExecRandom& e) {

	float randomNotesNum = score->getChannelInfo(e.ch).randomNotesNum;
	int randomChordNum = score->getChannelInfo(e.ch).randomChordNum;
	int velRange = score->getChannelInfo(e.ch).velocityRange;

	UCHAR b = e.bar > 3 ? grids.getBar() : e.bar;
	
	clear(b, e.ch, 3);
	
	int currentX = 0;
	for (int i = 0; i < BEAT; i++) {
		
		if (ofRandom(1.) < randomNotesNum) {
			
			int currentY = ofRandom(0, 8);
			for (int j = 0; j < randomChordNum; j++) {
				
				if (grids.get(b, e.ch)[i][currentY][0] == -1) {

					int vel = ofClamp(state.defaultVelocity + velRange * ofRandom(-1, 1), 0, 128);

					NoteModel n(e.ch, b, i, currentY, vel, 1, 3);
					
					int id = score->create(n);

					grids.get(b, e.ch)[i][currentY] = { id, 3 };

				}
				currentY += (int)ofRandom(0, 4);
				if (currentY > 15) currentY = 15;

			}
			
		}

		



	}


}