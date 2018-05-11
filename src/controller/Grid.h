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
		this->bar = bar;
	}

	void setChan(int ch) {
		this->ch = ch;

		float hue = 256 / CHANNEL * ch;
		for (int i = 0; i < color.size(); i++) {
			color[i].setHsb(hue, 256 * (0.7 - 0.1 * i), 255);
		}

		ofColor c;
		c.setHsb(hue, 255, 50);
		ofBackground(c);
	}

	int getBar() const { return bar; }
	int getChan() const { return ch; }
	const ofColor& getColor(int i) const { return color[i]; }

private:
	USHORT bar;
	USHORT ch;
	std::array<ofColor, 4> color;

	// bar - ch - x(beat) - y(pitch)
	vector<vector<Grid>> data;
};