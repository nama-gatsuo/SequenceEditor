#include "UIManager.h"

using namespace glm;

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

	size = ivec2(gridSize * score.getBeatCount(), gridSize * score.getPitchCount());

	ofAddListener(ofEvents().mouseMoved, this, &UIManager::mouseMoved);
	ofAddListener(ofEvents().mousePressed, this, &UIManager::mousePressed);
	ofAddListener(ofEvents().mouseDragged, this, &UIManager::mouseDragged);
	ofAddListener(ofEvents().mouseReleased, this, &UIManager::mouseReleased);
	ofAddListener(ofEvents().keyPressed, this, &UIManager::keyPressed);
	ofAddListener(ofEvents().mouseScrolled, this, &UIManager::mouseScrolled);

	ofAddListener(EventsEntity::execRandom, this, &UIManager::randomize);

	grids = GridUI(
		score.getChannelCount(),
		score.getBarCount(),
		score.getPitchCount(),
		score.getBeatCount()
	);
	grids.setBar(0);
	grids.setChan(0, this->score->getChannelInfo(0).hue);

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
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4::ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4::ImVec4(0, 0, 0, 0));
	
	// Selecting channel to edit
	ImGui::Begin("ChannelSelect", &isDraw, window_flags);
	ImGui::PushID("chselect");

	for (int i = 0; i < score->getChannelCount(); i++) {
		auto& col1 = score->getChannelInfo(i).levels[0].color;
		auto& col2 = score->getChannelInfo(i).levels[1].color;
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, col1);
		ImGui::PushStyleColor(ImGuiCol_Header, col2);
		
		bool b = grids.getChan() == i;
		if (ImGui::Selectable(ofToString(i).data(), &b, 0, ImVec2(30, 30))) {
			// Change channel to edit!
			grids.setChan(i, score->getChannelInfo(i).hue);
		}

		if (i % 8 != 7) ImGui::SameLine();
		ImGui::PopStyleColor(2);
	}
	ImGui::PopID();
	ImGui::End();
	
	
	// channel info
	ImGui::Begin("Channels", &isDraw, window_flags);
	score->getChannelInfo(grids.getChan()).drawGui();
	ImGui::End();

	// global info
	ImGui::Begin("Global", &isDraw, window_flags);
	int bar = score->getBarCount();
	ImGui::SliderInt("Loop", &bar, 1, 4);
	if (bar != score->getBarCount()) score->setBarCount(bar);

	int bpm = sequencer->getBpm();
	ImGui::SliderInt("BPM", &bpm, 80, 145);
	if (bpm != sequencer->getBpm()) sequencer->setBpm(bpm);

	ImGui::SliderInt("Velocity", &state.defaultVelocity, 1, 128);

	ImGui::PushID("edit");
	ImGui::Text("edit  :"); ImGui::SameLine();

	// Selecting level to edit
	int levNum = score->getChannelInfo(grids.getChan()).levelNum;
	
	for (int i = 0; i < levNum; i++) {
		auto& col = score->getChannelInfo(grids.getChan()).levels[i].color;

		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, col);
		ImGui::PushStyleColor(ImGuiCol_Header, col);
		bool b = state.currentEditLevel == i;
		if (ImGui::Selectable(ofToString(i).data(), &b, 0, ImVec2(30, 30))) {
			// Change level to edit!
			state.currentEditLevel = i;
		}
		
		if (i != 3) ImGui::SameLine();
		ImGui::PopStyleColor(2);
	}
	ImGui::PopID();

	ImGui::PushID("clear");
	ImGui::Text("clear :"); ImGui::SameLine();
	for (int i = 0; i < levNum; i++) {
		auto& col = score->getChannelInfo(grids.getChan()).levels[i].color;

		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, col);
		ImGui::PushStyleColor(ImGuiCol_Header, col);
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
		y = score->getPitchCount() - y - 1;

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
	uint8_t cbeat = sequencer->getCurrentBeat();
	uint8_t cbar = sequencer->getCurrentBar();
	uint8_t bar = grids.getBar();

	
	uint8_t y = score->getPitchCount(); // y count
	uint8_t x = score->getBeatCount(); // x count

	if (cbar == bar) {
		int o = 4;
		ofNoFill();
		ofDrawRectangle(cbeat * gridSize + o, o, gridSize - o * 2, gridSize * (y + 1) - o * 2);
		
		o = 8;
		ofFill();
		ofDrawRectangle(cbeat * gridSize + o, y * gridSize + o, gridSize - o * 2, gridSize - o * 2);
	}

	ofSetColor(255);
	for (int i = 0; i < x; i += 4) {
		ofDrawBitmapString(ofToString((int)bar) + "." + ofToString(i/4), i * gridSize, y * gridSize + 8);
	}

	// pitch guide
	for (int i = 0; i < y; i++) {
		int midiPitch = score->getChannelInfo(grids.getChan()).translateMidi(i);
		ofDrawBitmapString(ofToString(midiPitch), - 20, gridSize * i + 12);
	}

	// draw notes
	const auto& pairs = score->get(bar, grids.getChan());
	ofPushStyle();
	for (const auto& pair : pairs) {
		uint8_t ny = pair.second.y;
		uint8_t nx = pair.second.x;
		uint8_t level = pair.second.level;
		const auto& ci = score->getChannelInfo(grids.getChan());

		//if (ny >= y || nx >= x || level >= ci.levelNum) continue;

		bool isActive = ci.levels[level].isActive;
		int duration = pair.second.duration;
		int offset = (128.f - pair.second.velocity) / 128.f * gridSize * 0.5;

		ofSetColor(grids.getColor(level));

		if (isActive) ofFill();
		else ofNoFill();

		ofDrawRectangle(
			offset + (int)nx * gridSize, offset + (int)ny * gridSize,
			gridSize * duration - offset * 2, gridSize - offset * 2
		);
		

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
		auto& n = score->get(grids.getBar(), grids.getChan(), state.noteId);
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
		
		auto& n = score->get(grids.getBar(), grids.getChan(), state.noteId);
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
		if (ch >= score->getChannelCount()) ch -= score->getChannelCount();
		else if (ch < 0) ch += score->getChannelCount();

		grids.setChan(ch, score->getChannelInfo(ch).hue);
		
	} break;
	case OF_KEY_DOWN: {
		int ch = grids.getChan() - 1;
		if (ch >= score->getChannelCount()) ch -= score->getChannelCount();
		else if (ch < 0) ch += score->getChannelCount();

		grids.setChan(ch, score->getChannelInfo(ch).hue);
		
	} break;
	case OF_KEY_RIGHT: {
		int bar = grids.getBar() + 1;
		if (bar >= score->getBarCount()) bar -= score->getBarCount();
		else if (bar < 0) bar += score->getBarCount();

		grids.setBar(bar);
		
	} break;
	case OF_KEY_LEFT: {
		int bar = grids.getBar() - 1;
		if (bar >= score->getBarCount()) bar -= score->getBarCount();
		else if (bar < 0) bar += score->getBarCount();

		grids.setBar(bar);
		
	} break;
	default:
		break;
	}

}

bool UIManager::isMouseFormer(int x) const {
	x -= startPos.x;
	return (x / (int)(gridSize * 0.5)) % 2 == 0;
}

void UIManager::clear(uint8_t bar, uint8_t ch, uint8_t level) {
	
	auto& g = grids.get(bar, ch);
	
	for (uint8_t x = 0; x < score->getBeatCount(); x++) {
		for (uint8_t y = 0; y < score->getPitchCount(); y++) {
			uint8_t id = g[x][y][0];
			uint8_t lev = g[x][y][1];

			if (lev == level) {
				g[x][y] = { -1, -1 };
				uint8_t d = score->get(bar, ch)[id].duration;

				if (d >= score->getChannelCount() ||
					y >= score->getPitchCount() ||
					x >= score->getBeatCount()) continue;

				for (uint8_t i = 0; i < d; i++) {
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

	unsigned char b = e.bar > 3 ? grids.getBar() : e.bar;
	
	clear(b, e.ch, 3);
	
	int currentX = 0;
	for (int i = 0; i < score->getBeatCount(); i++) {
		
		if (ofRandom(1.) < randomNotesNum) {
			
			int currentY = 0;

			if (randomChordNum == 1) {
				currentY = ofRandom(-1, 16);
			} else {
				currentY = ofRandom(0, 8);
			}

			for (int j = 0; j < randomChordNum; j++) {

				if (grids.get(b, e.ch)[i][currentY][0] == -1) {

					int vel = ofClamp(state.defaultVelocity + velRange * ofRandom(-1, 1), 0, 128);

					NoteModel n{
						e.ch, b, i, currentY, vel, 1, 3
					};

					int id = score->create(n);

					grids.get(b, e.ch)[i][currentY] = { id, 3 };

				}
				currentY += (int)ofRandom(0, 4);
				if (currentY > 15) currentY = 15;

			}
			
		}

		



	}


}