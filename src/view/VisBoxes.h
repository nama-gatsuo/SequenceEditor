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
		UCHAR bar = sequnecer.getCurrentBar();
		UCHAR beat = sequnecer.getCurrentBeat();

		num = 0;
		ofPixels& colPix = colData.getPixelsRef();
		ofPixels& posPix = posData.getPixelsRef();
		ofPixels& unitPix = unitData.getPixelsRef();

		

		
		// bar block
		int barCount = 0;
		for (auto& bm : m) {
			
			bool isActiveBar = barCount == bar;
			
			// channel block
			int chIndex = 0;
			for (auto& ch : bm) {

				auto& chInfo = score.getChannelInfo(chIndex);

				for (auto& n : ch) {

					UCHAR x = n.second.x + barCount * 16;
					UCHAR y = n.second.y;
					UCHAR l = n.second.level;
					
					if (l > 3) continue;

					auto& c = chInfo.colors[l];
					colPix.setColor(num, 0, ofFloatColor(c.Value.x, c.Value.y, c.Value.z));
					posPix.setColor(num, 0, ofFloatColor(chIndex/64., y/64., x/64.));
					
					UCHAR ac = chInfo.isActive[l] ? 1 : 0;
					UCHAR v = n.second.velocity;
					UCHAR d = n.second.duration;
					UCHAR at = (isActiveBar && x == beat) ? 1 : 0;

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

	void draw(const ofCamera& cam, bool isShadow) {

		
		shader.begin();
		shader.setUniformTexture("col", colData.getTexture(), 0);
		shader.setUniformTexture("pos", posData.getTexture(), 1);
		shader.setUniformTexture("unit", unitData.getTexture(), 2);
		shader.setUniform1i("num", num);
		shader.setUniform1f("size", size);

		shader.setUniform1f("farClip", cam.getFarClip());
		shader.setUniform1f("nearClip", cam.getNearClip());
		shader.setUniform1i("isShadow", isShadow ? 1 : 0);

		mesh.drawInstanced(OF_MESH_FILL, num);

		shader.end();
		
	}
private:
	float size;
	ofShader shader;
	ofVboMesh mesh;

	int num;
	ofImage colData;
	ofImage posData;
	ofImage unitData;

};