#include "ChannelInfo.h"

const std::vector<std::vector<int>> ChannelInfo::scaleStep{
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
	{ 0, 2, 4, 5, 7, 9, 11 },
	{ 0, 2, 3, 5, 7, 8, 10 },
	{ 0, 2, 4, 7, 9 }
};

ChannelInfo::ChannelInfo() :
	channelIndex(-1), hue(0),
	isRandomLoop(false), randomChordNum(1), randomNotesNum(1), velocityRange(10) {}

ChannelInfo::ChannelInfo(int index, const string& name, float hue) :
	channelIndex(index), hue(hue),
	isRandomLoop(false), randomChordNum(1), randomNotesNum(1), velocityRange(10),
	name("ch[" + ofToString(index) + "]: " + name),
	channelIndexInDAW(index + 1)
{
	colorHeader = ImColor::HSV(hue, 0.4, 0.4);
	
	int levelNum = 4;

	for (int i = 0; i < levelNum; i++) {
		LevelInfo l{
			i == 0,
			ImColor::HSV(hue, 1. - 0.25 * i, 0.4 + 0.08 * i)
		};
		levels.push_back(std::move(l));
	}

}

void ChannelInfo::drawGui() {

	ImGui::PushID(channelIndex);
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colorHeader);
	ImGui::PushStyleColor(ImGuiCol_Header, colorHeader);
	ImGui::PopStyleColor(2);

	ImGui::Text(name.data());

	ImGui::NewLine();
		
	ImGui::PushID("active");
	ImGui::Text("active:"); ImGui::SameLine();
	for (int i = 0; i < levels.size(); i++) {
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, levels[i].color);
		ImGui::PushStyleColor(ImGuiCol_Header, levels[i].color);
		ImGui::Selectable(ofToString(i).data(), &levels[i].isActive, 0, ImVec2(30, 30));
		if (i != 3) ImGui::SameLine();
		ImGui::PopStyleColor(2);
	}
	ImGui::PopID();

	ImGui::NewLine();

	int scaleNum = static_cast<int>(scale);
	ImGui::Combo("scale", &scaleNum, "Chromatic\0Major\0Minor\0Pentatonic");
	scale = static_cast<Scale>(scaleNum);
		
	int keyNum = static_cast<int>(key);
	ImGui::Combo("key", &keyNum, "C\0Db\0D\0Eb\0E\0F\0Gb\0G\0Ab\0A\0Bb\0B");
	key = static_cast<Key>(keyNum);
		
	ImGui::SliderInt("octave", &octave, -2, 8);
	
	ImGui::NewLine();

	ImGui::Text("Random");
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, levels[0].color);
	ImGui::PushStyleColor(ImGuiCol_Header, levels[1].color);
	ImGui::Selectable("Loop", &isRandomLoop, 0, ImVec2(60, 30));
	bool b = false;
	if (ImGui::Selectable("Exec", &b, 0, ImVec2(60, 30))) {
		ExecRandom e(10, channelIndex);
		ofNotifyEvent(EventsEntity::execRandom, e);
	}

	ImGui::PopStyleColor(2);
	ImGui::SliderInt("chord", &randomChordNum, 1, 8);
	ImGui::SliderFloat("notesNum", &randomNotesNum, 0, 1);
	ImGui::SliderInt("velocityRange", &velocityRange, 0, 60);

	ImGui::PopID();

}

uint8_t ChannelInfo::translateMidi(int h) const {
	int oct = octave + 2;
	int k = static_cast<int>(key);
	int si = static_cast<int>(scale);
	int ss = scaleStep[si].size();
	
	int yo = (h / ss + oct) * 12 + k;
	int yp = ChannelInfo::scaleStep[si][h % ss];
	
	return yo + yp;
}
