#include "UIState.h"

using namespace glm;

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
		const NoteModel& cn = score->get(grids->getBar(), grids->getChan(), noteId);
		NoteModel nn = cn;

		int v = nn.velocity + y * 4;
		if (v < 0) nn.velocity = 0;
		else if (v > 128) nn.velocity = 128;
		else nn.velocity = v;

		int id = score->update(grids->getBar(), grids->getChan(), noteId, nn);
		for (int i = 0; i < nn.duration; i++) {
			grids->get()[nn.x + i][nn.y][0] = id;
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
		const auto& n = score->get(grids->getBar(), grids->getChan(), noteId);
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
			const NoteModel& cn = score->get(grids->getBar(), grids->getChan(), noteId);
			if (cn.x >= score->getBeatCount() || cn.y > score->getPitchCount()) break;
			int oldDur = cn.duration;

			int newDur = current.x - cn.x + 1;
			if (newDur < 1) newDur = 1;
			
			// check if there is any notes in new duration
			if (newDur > oldDur) {
				for (int x = cn.x + oldDur + 1; x < cn.x + newDur; x++) {
					int id = grids->get()[x][cn.y][0];
					if (id != -1) {
						newDur = x - cn.x;
						break;
					}
				}
			}

			NoteModel nn = cn;
			nn.duration = newDur;

			noteId = score->update(grids->getBar(), grids->getChan(), noteId, nn);

			// rewrite ids on the grid
			if (newDur > oldDur) {
				for (int x = nn.x; x < nn.x + newDur; x++) {
					grids->get()[x][nn.y][0] = noteId;
				}
			} else {
				for (int d = 0; d < oldDur; d++) {
					if (d < newDur) {
						grids->get()[nn.x + d][nn.y][0] = noteId;
					} else {
						grids->get()[nn.x + d][nn.y][0] = -1;
					}
					
				}
			}

			code = Code::HOVER_NOTE;
			pressed = ivec2(-1);

		} break;
	case Code::DRAG_DELETE:
		// delete
		if (all(equal(current, pressed))) {
			
			const NoteModel& n = score->get(grids->getBar(), grids->getChan(), noteId);
			for (int x = n.x; x < n.x + n.duration; x++) {
				grids->get()[x][n.y] = { -1, -1 };
			}
			score->remove(grids->getBar(), grids->getChan(), noteId);
			
		}

		// reset
		code = Code::HOVER_EMPTY;
		pressed = ivec2(-1);
		break;
	default: break;
	}
}