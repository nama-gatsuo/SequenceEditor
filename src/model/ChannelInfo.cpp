#include "ChannelInfo.h"

std::vector<vector<int>> ChannelInfo::scaleStep;

ChannelInfo::ChannelInfo() {}

ChannelInfo::ChannelInfo(int i, const string& name) : chIndex(i) {
	chNumInDAW = chIndex + 1;
	this->name = "ch[" + ofToString(i) + "]: " + name;
	
	float hue = 1.f / 16 * chIndex;
	for (int i = 0; i < colors.size(); i++) {
		colors[i] = ImColor::HSV(hue, 1. - 0.1 * i, 0.8);
	}
}

void ChannelInfo::drawGui() {

	ImGui::PushID(chIndex);
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colors[0]);
	ImGui::PushStyleColor(ImGuiCol_Header, colors[3]);

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