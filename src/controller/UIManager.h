#include "ofMain.h"
#include "ScoreManager.h"
#include "Sequencer.h"
#include "Grid.h"
#include "UIState.h"

#include "ofxImGui.h"

class UIManager {
public:
	UIManager(int gridSize = 40);
	~UIManager();

	void setup(ScoreManager& score, Sequencer& sequencer);
	void setPosition(int x, int y) { startPos = glm::ivec2(x, y); }
	void draw(int offsetX, int offsetY);

	void mouseMoved(ofMouseEventArgs& args);
	void mousePressed(ofMouseEventArgs& args);
	void mouseDragged(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);
	void mouseScrolled(ofMouseEventArgs& args);
	void keyPressed(ofKeyEventArgs& args);

private:

	glm::ivec2 translateMousePos(int x, int y);
	void drawGrid() const;
	void drawStateInfo() const;
	bool isMouseFormer(int x) const;

	void clear(uint8_t bar, uint8_t ch, uint8_t level);
	void randomize(ExecRandom& e);


	ScoreManager * score;
	Sequencer * sequencer;

	int gridSize;
	glm::ivec2 startPos;
	glm::ivec2 size;

	UIState state;
	
	GridUI grids;
	ofxImGui::Gui gui;

	ofTrueTypeFont font;

};