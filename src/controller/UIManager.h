#include "ofMain.h"
#include "ScoreManager.h"
#include "Sequencer.h"
#include "Grid.h"
#include "GlobalConstants.h"
#include "UIState.h"

#include "ofxImGui.h"

using namespace glm;

class UIManager {
public:
	UIManager(int gridSize = 40, int defaultVelocity = 100);
	~UIManager();

	void setup(ScoreManager& score, Sequencer& sequencer);
	void setPosition(int x, int y) { startPos = ivec2(x, y); }
	void draw(int offsetX, int offsetY);

	void mouseMoved(ofMouseEventArgs& args);
	void mousePressed(ofMouseEventArgs& args);
	void mouseDragged(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);
	void mouseScrolled(ofMouseEventArgs& args);
	void keyPressed(ofKeyEventArgs& args);

private:

	ivec2 translateMousePos(int x, int y);
	void drawGrid() const;
	void drawStateInfo() const;
	bool isMouseFormer(int x) const;

	ScoreManager * score;
	Sequencer * sequencer;

	int gridSize;
	ivec2 startPos;
	ivec2 size;
	int defaultVelocity;

	UIState state;
	
	GridUI grids;
	ofxImGui::Gui gui;

	ofTrueTypeFont font;

};