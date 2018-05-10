#pragma once
#include "ofMain.h"
#include "ScoreManager.h"
#include "Grid.h"

using namespace glm;

class UIState {
public:
	//UIState();
	void setup(GridUI& grids, ScoreManager& score);

	enum struct Code {
		FREE, HOVER_EMPTY, HOVER_NOTE, DRAG_EDIT, DRAG_CREATE
	} code;

	ivec2 current;
	ivec2 pressed;
	int noteId;

	void onMouseMoved(float x, float y, const ivec2& index);
	void onMouseScrolled(float y);
	void onMousePressed();
	void onMouseDragged(float x, float y, const ivec2& index);
	void onMouseReleased();

	void drawInfo();

private:
	vec2 mouse;
	GridUI * grids;
	ScoreManager * score;

};