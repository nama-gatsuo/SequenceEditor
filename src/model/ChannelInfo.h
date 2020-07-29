#pragma once

#include "Events.h"
#include "ofxImGui.h"

class ChannelInfo {
public:
	ChannelInfo();
	ChannelInfo(int index, const std::string& name, float hue);

	void drawGui();
	uint8_t translateMidi(int midi) const;

	enum Scale {
		CHROMATIC = 0, MAJOR, MINOR, PENTATONIC
	} scale;

	enum Key {
		C = 0, D_FLAT, D, E_FLAT, E, F,
		G_FLAT, G, A_FLAT, A, B_FLAT, B
	} key;

	int octave;
	
	static const std::vector<std::vector<int>> scaleStep;

	uint8_t channelIndex;
	uint8_t channelIndexInDAW;
	std::string name;
	float hue;

	uint8_t levelNum;
	struct LevelInfo {
		bool isActive;
		ImVec4 color;
	};

	std::vector<LevelInfo> levels;
	ImVec4 colorHeader;

	bool isRandomLoop;
	int randomChordNum;
	float randomNotesNum;
	int velocityRange;

};