#include "UIState.h"

void UIState::setup(GridUI& grids, ScoreManager& score) {
	this->grids = &grids;
	this->score = &score;
	code = Code::FREE;
	noteId = -1;
}

void UIState::onMouseMoved(const ivec2& index) {
	
	current = index;

	bool isHoverOnGrid = !all(lessThan(current, ivec2(0)));

	switch (code) {
	case Code::FREE:
		if (isHoverOnGrid) {
			noteId = grids->get()[current.x][current.y][0];
			if (noteId < 0) {
				code = Code::HOVER_EMPTY;
			} else {
				code = Code::HOVER_NOTE;
			}
		}
		break;
	case Code::HOVER_EMPTY:
		if (isHoverOnGrid) {
			noteId = grids->get()[current.x][current.y][0];
			if (noteId != -1) {
				code = Code::HOVER_NOTE;
			}
		} else {
			code = Code::FREE;
		}
		break;
	case Code::HOVER_NOTE:
		if (isHoverOnGrid) {
			noteId = grids->get()[current.x][current.y][0];
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
		// copy from reference
		// because original data will be deleted in score->update()
		NoteModel n = score->get(noteId);
		
		int v = n.velocity + y * 4;
		if (v < 0) n.velocity = 0;
		else if (v > 128) n.velocity = 128;
		else n.velocity = v;

		int id = score->update(noteId, n);
		for (int i = 0; i < n.duration; i++) {
			grids->get()[n.x + i][n.y][0] = id;
		}
		noteId = id;
	}
}

void UIState::onMousePressed(bool isFormer) {
	pressed = current;

	switch (code) {
	case Code::HOVER_EMPTY:
		code = Code::DRAG_CREATE;
		break;
	case Code::HOVER_NOTE: {
		NoteModel& n = score->get(noteId);
		if (all(equal(pressed, ivec2(n.x, n.y))) && isFormer) {
			code = Code::DRAG_DELETE;
		} else {
			code = Code::DRAG_EDIT;
		}
	} break;
	default: break;
	}
}

void UIState::onMouseDragged(const ivec2& index) {
	
	current = index;

	if (code == Code::DRAG_CREATE) {
		current.y = pressed.y;
		if (current.x < 0) current.x = pressed.x;
	} else if (code == Code::DRAG_EDIT) {
		current.y = pressed.y;
		if (current.x < 0) current.x = pressed.x;
	}
	
}

void UIState::onMouseReleased() {
	
	switch (code) {
	case Code::DRAG_CREATE: {
		if (all(lessThan(current, ivec2(0)))) {
			// mouse cursor is out of the grid, reset.
			noteId = -1;
			code = Code::FREE;

		} else {

			// create
			int dragLength = abs(pressed.x - current.x) + 1;
			bool isDragBack = pressed.x < current.x;

			int x = pressed.x;
			int start = x;
			int y = pressed.y;
			int d = 0;

			// check if there is any notes in new duration
			for (int i = 0; i < dragLength; i++) {
				int id = grids->get()[isDragBack ? start + i : start - i][y][0];
				if (id > -1) break;
				else {
					x = isDragBack ? start + i : start - i;
					d++;
				}
			}

			if (isDragBack) x = start;

			NoteModel n{ 
				grids->getChan(),
				grids->getBar(),
				x, y, 
				defaultVelocity,
				d,
				currentEditLevel
			};

			int id = score->create(n);
			for (int i = 0; i < d; i++) {
				grids->get()[x + i][y] = { id, currentEditLevel };
			}

			// reset
			code = Code::HOVER_NOTE;
			noteId = id;

		}
		
		pressed = ivec2(-1);
		
	} break;
	case Code::DRAG_EDIT:
		if (all(lessThan(current, ivec2(0)))) {
			// mouse cursor is out of the grid, reset.
			noteId = -1;
			code = Code::FREE;
			pressed = ivec2(-1);
		} else {
			// copy from reference
			// because original data will be deleted in score->update()
			NoteModel n = score->get(noteId);
			if (n.x > 15 || n.y > 15) break;
			int oldDur = n.duration;

			int newDur = current.x - n.x + 1;
			if (newDur < 1) newDur = 1;
			
			// check if there is any notes in new duration
			if (newDur > oldDur) {
				for (int x = n.x + oldDur + 1; x < n.x + newDur; x++) {
					int id = grids->get()[x][n.y][0];
					if (id != -1) {
						newDur = x - n.x;
						break;
					}
				}
			}

			n.duration = newDur;

			noteId = score->update(noteId, n);

			// rewrite ids on the grid
			if (newDur > oldDur) {
				for (int x = n.x; x < n.x + newDur; x++) {
					grids->get()[x][n.y][0] = noteId;
				}
			} else {
				for (int d = 0; d < oldDur; d++) {
					if (d < newDur) {
						grids->get()[n.x + d][n.y][0] = noteId;
					} else {
						grids->get()[n.x + d][n.y][0] = -1;
					}
					
				}
			}

			code = Code::HOVER_NOTE;
			pressed = ivec2(-1);

		} break;
	case Code::DRAG_DELETE:
		// delete
		if (all(equal(current, pressed))) {
			
			NoteModel& n = score->get(noteId);
			for (int x = n.x; x < n.x + n.duration; x++) {
				grids->get()[x][n.y] = { -1, -1 };
			}
			score->remove(noteId);
			
		}

		// reset
		code = Code::HOVER_EMPTY;
		pressed = ivec2(-1);
		break;
	default: break;
	}
}