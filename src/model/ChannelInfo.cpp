#include "ChannelInfo.h"

std::vector<vector<int>> ChannelInfo::scaleStep;

ChannelInfo::ChannelInfo() {}

ChannelInfo::ChannelInfo(int i, const string& name) : chIndex(i) {
	chNumInDAW = chIndex + 1;
	this->name = "ch[" + ofToString(i) + "]: " + name;
	
	float hue = ((1. / 3.) * (i % 3) + (i / 3) * 0.03) * 0.5 + 0.4;
	colorHeader = ImColor::HSV(hue, 0.4, 0.4);
	for (int i = 0; i < colors.size(); i++) {
		colors[i] = ImColor::HSV(hue, 1. - 0.25 * i, 0.4 + 0.08 * i);
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
	ImGui::PopStyleColor(2);

	ImGui::NewLine();
		
	ImGui::PushID("active");
	ImGui::Text("active:"); ImGui::SameLine();
	for (int i = 0; i < 4; i++) {
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colors[i]);
		ImGui::PushStyleColor(ImGuiCol_Header, colors[i]);
		ImGui::Selectable(ofToString(i).data(), &isActive[i], 0, ImVec2(30, 30));
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
