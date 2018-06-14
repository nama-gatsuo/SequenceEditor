#pragma once
#include "ofMain.h"
#include "ScoreManager.h"
#include "Grid.h"
#include "Events.h"

using namespace glm;

class UIState {
public:
	//UIState();
	void setup(GridUI& grids, ScoreManager& score);

	enum struct Code {
		FREE, HOVER_EMPTY, HOVER_NOTE, DRAG_DELETE, DRAG_CREATE, DRAG_EDIT
	} code;

	int currentEditLevel;
	ivec2 current;
	ivec2 pressed;
	int noteId;

	void onMouseMoved(const ivec2& index);
	void onMouseScrolled(float y);
	void onMousePressed(bool isFormer);
	void onMouseDragged(const ivec2& index);
	void onMouseReleased();

	void drawInfo();

private:
	GridUI * grids;
	ScoreManager * score;

};