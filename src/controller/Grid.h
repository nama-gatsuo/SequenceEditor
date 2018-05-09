#pragma once
#include "ofMain.h"
#include "GlobalConstants.h"

class Grid {
public:
	Grid() {}
	Grid(int x, int y) : xSize(x), ySize(y) {
		ids.assign(xSize, vector<int>(ySize, -1));
	}

	vector<int>& operator[](int x) {
		return ids[x];
	}

private:
	int xSize;
	int ySize;
	vector<vector<int>> ids;
};

class GridUI {
public:
	GridUI() {};
	GridUI(USHORT ch, USHORT bar, USHORT pitch, USHORT beat) : ch(ch), bar(bar) {
		data.assign(bar, vector<Grid>(ch, Grid(beat, pitch)));
	}
	
	Grid& get() {
		return data[bar][ch];
	}
	
	void setBar(int bar) {
		if (bar >= BAR) bar -= BAR;
		else if (bar < 0) bar += BAR;

		this->bar = bar;
		
	} 
	void setChan(int ch) {
		if (ch >= CHANNEL) ch -= CHANNEL;
		else if (ch < 0) ch += CHANNEL;
		this->ch = ch;

		float hue = 256 / CHANNEL * ch;
		color[0].setHsb(hue, 256 * (1. - 0.1 * 1), 200);
		color[1].setHsb(hue, 256 * (1. - 0.1 * 2), 200);
		color[2].setHsb(hue, 256 * (1. - 0.1 * 3), 200);
		color[3].setHsb(hue, 256 * (1. - 0.1 * 4), 200);
		
	}

	int getBar() { return bar; }
	int getChan() { return ch; }
	ofColor& getColor(int i) { return color[i]; }

private:
	USHORT bar;
	USHORT ch;
	std::array<ofColor, 4> color;

	// bar - ch - x(beat) - y(pitch)
	vector<vector<Grid>> data;
};