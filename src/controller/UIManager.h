#include "ofMain.h"
#include "ScoreManager.h"
#include "Sequencer.h"
#include "Grid.h"
#include "GlobalConstants.h"

using namespace glm;

class UIManager {
public:
	UIManager(int gridSize = 32);
	~UIManager();

	void setup(ScoreManager& score, Sequencer& sequencer);
	void setPosition(int x, int y) { startPos = ivec2(x, y); }
	void draw(int offsetX, int offsetY);

	void mouseMoved(ofMouseEventArgs& args);
	/*void mousePressed(ofMouseEventArgs& args);
	void mouseDragged(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);*/
	void keyPressed(ofKeyEventArgs& args);

private:

	ivec2 translateMousePos(int x, int y);
	void drawGrid();

	ScoreManager * score;
	Sequencer * sequencer;

	int gridSize;
	ivec2 startPos;
	ivec2 size;

	struct HoverInfo {
		bool isGridHover;
		ivec2 gridIndex;

		bool isNoteHover;
		int noteId;
	} hover;

	struct DragInfo {
		bool isDragged;
		ivec2 pressedIndex;
		ivec2 currentIndex;
		ivec2 releaseIndex;
	};

	GridUI grids;

};