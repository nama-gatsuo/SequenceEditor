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
	GridUI(UCHAR ch, UCHAR bar, UCHAR pitch, UCHAR beat) : ch(ch), bar(bar) {
		data.assign(bar, vector<Grid>(ch, Grid(beat, pitch)));
	}

	Grid& get() {
		return data[bar][ch];
	}

	void setBar(UCHAR bar) {
		this->bar = bar;
	}

	void setChan(UCHAR ch) {
		this->ch = ch;

		float hue = 256 * (((1. / 3.) * (ch % 3) + (ch / 3) * 0.03) * 0.5 + 0.4);
		for (int i = 0; i < color.size(); i++) {
			color[i].setHsb(hue, 256 * (1. - 0.25 * i), 256 * 0.8);
		}

		ofColor c;
		c.setHsb(hue, 255 * 0.4, 255 * 0.2);
		ofBackground(c);
	}

	UCHAR getBar() const { return bar; }
	UCHAR getChan() const { return ch; }
	const ofColor& getColor(int i) const { return color[i]; }

private:
	UCHAR bar;
	UCHAR ch;
	std::array<ofColor, 4> color;

	// bar - ch - x(beat) - y(pitch)
	vector<vector<Grid>> data;
};