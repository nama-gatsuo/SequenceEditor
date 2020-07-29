#pragma once

#include "ScoreManager.h"
#include "Grid.h"
#include "Events.h"

class UIState {
public:
	//UIState();
	void setup(GridUI& grids, ScoreManager& score);

	enum struct Code {
		FREE, HOVER_EMPTY, HOVER_NOTE, DRAG_DELETE, DRAG_CREATE, DRAG_EDIT
	} code;

	int currentEditLevel = 0;
	int defaultVelocity = 64;

	glm::ivec2 current;
	glm::ivec2 pressed;
	int noteId;

	void onMouseMoved(const glm::ivec2& index);
	void onMouseScrolled(float y);
	void onMousePressed(bool isFormer);
	void onMouseDragged(const glm::ivec2& index);
	void onMouseReleased();

private:
	GridUI * grids;
	ScoreManager * score;

};