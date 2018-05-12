#include "ChannelInfo.h"

std::vector<vector<int>> ChannelInfo::scaleStep;

ChannelInfo::ChannelInfo() {}

ChannelInfo::ChannelInfo(int i, const string& name) : chIndex(i) {
	chNumInDAW = chIndex + 1;
	this->name = "ch[" + ofToString(i) + "]: " + name;
	
	float hue = ((1. / 3.) * (i % 3) + (i / 3) * 0.03) * 0.5 + 0.4;
	colorHeader = ImColor::HSV(hue, 0.4, 0.4);
	for (int i = 0; i < colors.size(); i++) {
		colors[i] = ImColor::HSV(hue, 1. - 0.25 * i, 0.8);
	}

	isActive[0] = true;
	isActive[1] = false;
	isActive[2] = false;
	isActive[3] = false;

}

void ChannelInfo::drawGui() {

	ImGui::PushID(chIndex);
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colorHeader);
	ImGui::PushStyleColor(ImGuiCol_Header, colorHeader);

	if (ImGui::CollapsingHeader(name.data())) {

		ImGui::PushStyleColor(ImGuiCol_CheckMark, colors[0]);
		ImGui::Checkbox("base", &isActive[0]); ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_CheckMark, colors[1]);
		ImGui::Checkbox("fill1", &isActive[1]); ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_CheckMark, colors[2]);
		ImGui::Checkbox("fill2", &isActive[2]); ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_CheckMark, colors[3]);
		ImGui::Checkbox("fill3", &isActive[3]);

		int scaleNum = static_cast<int>(scale);
		ImGui::Combo("scale", &scaleNum, "Chromatic\0Major\0Minor\0Pentatonic");
		scale = static_cast<Scale>(scaleNum);
		
		int keyNum = static_cast<int>(key);
		ImGui::Combo("key", &keyNum, "C\0Db\0D\0Eb\0E\0F\0Gb\0G\0Ab\0A\0Bb\0B");
		key = static_cast<Key>(keyNum);
		
		ImGui::SliderInt("octave", &octave, -2, 8);
		
		ImGui::PopStyleColor(4);
		
	}
	ImGui::PopStyleColor(2);
	ImGui::PopID();

}

UCHAR ChannelInfo::translateMidi(int h) const {
	int oct = octave + 2;
	int k = static_cast<int>(key);
	int si = static_cast<int>(scale);
	int ss = scaleStep[si].size();
	
	int yo = (h / ss + oct) * 12 + k;
	int yp = ChannelInfo::scaleStep[si][h % ss];
	
	return yo + yp;
}