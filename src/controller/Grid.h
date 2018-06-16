#pragma once
#include "ofMain.h"
#include "GlobalConstants.h"

class Grid {
public:
	Grid() {}
	Grid(int x, int y) : xSize(x), ySize(y) {
		ids.assign(xSize, vector<array<int, 2>>(ySize, array<int, 2>{-1, -1}));
	}

	vector<array<int, 2>>& operator[](int x) {
		return ids[x];
	}

private:
	int xSize;
	int ySize;
	// x - y - { id, level }
	vector<vector<array<int, 2>>> ids;
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

	void setChan(UCHAR ch, float hue) {
		this->ch = ch;
		
		for (int i = 0; i < color.size(); i++) {
			color[i].setHsb(hue, 1. - 0.25 * i, 0.4 + 0.08 * i);
		}

		ofBackground(color[2] * 0.2);
	}

	UCHAR getBar() const { return bar; }
	UCHAR getChan() const { return ch; }
	const ofFloatColor& getColor(int i) const { return color[i]; }

private:
	UCHAR bar;
	UCHAR ch;
	std::array<ofFloatColor, 4> color;

	// bar - ch - x(beat) - y(pitch)
	vector<vector<Grid>> data;
};