#pragma once
#include "ofColor.h"

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
	GridUI(uint8_t ch, uint8_t bar, uint8_t pitch, uint8_t beat) : ch(ch), bar(bar) {
		data.assign(bar, std::vector<Grid>(ch, Grid(beat, pitch)));
	}

	Grid& get() {
		return data[bar][ch];
	}

	Grid& get(uint8_t bar, uint8_t ch) {
		return data[bar][ch];
	}

	void setBar(uint8_t bar) {
		this->bar = bar;
	}

	void setChan(uint8_t ch, float hue) {
		this->ch = ch;
		
		for (int i = 0; i < color.size(); i++) {
			color[i].setHsb(hue, 1. - 0.25 * i, 0.4 + 0.08 * i);
		}

		ofBackground(color[2] * 0.2);
	}

	uint8_t getBar() const { return bar; }
	uint8_t getChan() const { return ch; }
	const ofFloatColor& getColor(int i) const { return color[i]; }

private:
	uint8_t bar;
	uint8_t ch;
	std::array<ofFloatColor, 4> color;

	// bar - ch - x(beat) - y(pitch)
	std::vector<std::vector<Grid>> data;
};