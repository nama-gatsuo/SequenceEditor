#pragma once
#include "ofMain.h"
#include "ScoreManager.h"

class VisBox {
public:
	void setup() {
		size = 60.;
		mesh = ofMesh::box(size, size, size);
		shader.load("shader/scene/box");

		colData.allocate(2048, 1, OF_IMAGE_COLOR);
		posData.allocate(2048, 1, OF_IMAGE_COLOR);
		unitData.allocate(2048, 1, OF_IMAGE_COLOR_ALPHA);
		num = 0;
	}

	void update(ScoreManager& score, Sequencer& sequnecer) {
		auto& m = score.getNotes();
		uint8_t bar = sequnecer.getCurrentBar();
		uint8_t beat = sequnecer.getCurrentBeat();

		num = 0;
		ofPixels& colPix = colData.getPixelsRef();
		ofPixels& posPix = posData.getPixelsRef();
		ofPixels& unitPix = unitData.getPixelsRef();
		
		barCount = score.getBarCount();
		// bar block
		int barCount = 0;
		for (auto& bm : m) {
			
			bool isActiveBar = barCount == bar;
			
			// channel block
			int chIndex = 0;
			for (auto& ch : bm) {

				auto& chInfo = score.getChannelInfo(chIndex);

				for (auto& n : ch) {

					uint8_t x = n.second.x;
					uint8_t y = n.second.y;
					uint8_t l = n.second.level;
					
					if (l > 3) continue;

					const auto& c = chInfo.levels[l].color;
					colPix.setColor(num, 0, ofFloatColor(c.x, c.y, c.z));
					posPix.setColor(num, 0, ofFloatColor(chIndex/64., y/64., (x + barCount * 16) /64.));
					
					uint8_t ac = chInfo.levels[l].isActive ? 1 : 0;
					uint8_t v = n.second.velocity;
					uint8_t d = n.second.duration;
					uint8_t at = (isActiveBar && x == beat) ? 1 : 0;

					unitPix.setColor(num, 0, ofFloatColor(ac, v/128., d/16., at));

					num++;
				}

				chIndex++;
			}

			barCount++;
		}
		colData.setFromPixels(colPix);
		posData.setFromPixels(posPix);
		unitData.setFromPixels(unitPix);
	}

	void draw(float lds, bool isShadow) {

		
		shader.begin();
		shader.setUniformTexture("col", colData.getTexture(), 0);
		shader.setUniformTexture("pos", posData.getTexture(), 1);
		shader.setUniformTexture("unit", unitData.getTexture(), 2);
		shader.setUniform1i("num", num);
		shader.setUniform1i("barCount", barCount);
		shader.setUniform1f("size", size);
		

		shader.setUniform1f("lds", lds);
		shader.setUniform1i("isShadow", isShadow ? 1 : 0);

		mesh.drawInstanced(OF_MESH_FILL, num);

		shader.end();
		
	}
private:
	float size;
	int barCount;
	ofShader shader;
	ofVboMesh mesh;

	int num;
	ofImage colData;
	ofImage posData;
	ofImage unitData;

};