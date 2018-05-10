#include "UIState.h"

void UIState::setup(GridUI& grids, ScoreManager& score) {
	this->grids = &grids;
	this->score = &score;
	code = Code::FREE;
}

void UIState::onMouseMoved(float x, float y, const ivec2& index) {
	mouse.x = x;
	mouse.y = y;

	current = index;

	bool isHoverOnGrid = !any(lessThan(current, ivec2(0)));

	switch (code) {
	case Code::FREE:
		if (isHoverOnGrid) {
			noteId = grids->get()[current.x][current.y];
			if (noteId < 0) {
				code = Code::HOVER_EMPTY;
			} else {
				code = Code::HOVER_NOTE;
			}
		}
		break;
	case Code::HOVER_EMPTY:
		if (isHoverOnGrid) {
			noteId = grids->get()[current.x][current.y];
			if (noteId != -1) {
				code = Code::HOVER_NOTE;
			}
		} else {
			code = Code::FREE;
		}
		break;
	case Code::HOVER_NOTE:
		if (isHoverOnGrid) {
			noteId = grids->get()[current.x][current.y];
			if (noteId == -1) {
				code = Code::HOVER_EMPTY;
			}
		} else {
			code = Code::FREE;
		}
		break;
	default: break;
	}

}

void UIState::onMouseScrolled(float y) {
	if (code == Code::HOVER_NOTE) {
		NoteModel& n = score->get(noteId);
		n.velocity -= y * 4;

		score->update(noteId, n);

	}
}

void UIState::onMousePressed() {
	pressed = current;

	switch (code) {
	case Code::HOVER_EMPTY:
		code = Code::DRAG_CREATE;
		break;
	case Code::HOVER_NOTE:
		code = Code::DRAG_EDIT;
		break;
	default: break;
	}
}

void UIState::onMouseDragged(float x, float y, const ivec2& index) {
	current = index;
}

void UIState::onMouseReleased() {
	
	switch (code) {
	case Code::DRAG_CREATE: {
		// create
		int dragLength = abs(pressed.x - current.x) + 1;
		bool isDragBack = pressed.x < current.x;

		int x = pressed.x;
		int start = x;
		int y = pressed.y;
		int d = 0;

		for (int i = 0; i < dragLength; i++) {
			int id = grids->get()[isDragBack ? start + i : start - i][y];
			if (id > -1) break;
			else {
				x = isDragBack ? start + i : start - i;
				d++;
			}
		}

		if (isDragBack) x = start;

		NoteModel n(grids->getChan(), grids->getBar(), x, y, 100, d);
		int id = score->create(n);
		for (int i = 0; i < d; i++) {
			grids->get()[x + i][y] = id;
		}

		// reset
		code = UIState::Code::HOVER_NOTE;
		pressed = ivec2(-1);
		noteId = id;
	} break;
	case UIState::Code::DRAG_EDIT:
		// edit


		// reset
		code = UIState::Code::HOVER_NOTE;
		pressed = ivec2(-1);
		break;
	default: break;
	}
}

void UIState::drawInfo() {

}