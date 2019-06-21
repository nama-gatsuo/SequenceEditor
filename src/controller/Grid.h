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
	GridUI(unsigned char ch, unsigned char bar, unsigned char pitch, unsigned char beat) : ch(ch), bar(bar) {
		data.assign(bar, vector<Grid>(ch, Grid(beat, pitch)));
	}

	Grid& get() {
		return data[bar][ch];
	}

	Grid& get(unsigned char bar, unsigned char ch) {
		return data[bar][ch];
	}

	void setBar(unsigned char bar) {
		this->bar = bar;
	}

	void setChan(unsigned char ch, float hue) {
		this->ch = ch;
		
		for (int i = 0; i < color.size(); i++) {
			color[i].setHsb(hue, 1. - 0.25 * i, 0.4 + 0.08 * i);
		}

		ofBackground(color[2] * 0.2);
	}

	unsigned char getBar() const { return bar; }
	unsigned char getChan() const { return ch; }
	const ofFloatColor& getColor(int i) const { return color[i]; }

private:
	unsigned char bar;
	unsigned char ch;
	std::array<ofFloatColor, 4> color;

	// bar - ch - x(beat) - y(pitch)
	vector<vector<Grid>> data;
};