#include "ofMain.h"
#include "ofApp.h"
#include "VisualApp.h"

//========================================================================
int main( ){

	
	ofGLFWWindowSettings s;
	s.setGLVersion(4, 1);
	
	// ui
	s.setSize(1024, 768);
	s.resizable = true;
	s.setPosition(glm::vec2(0, 10));
	shared_ptr<ofAppBaseWindow> main = ofCreateWindow(s);
	shared_ptr<ofApp> mainApp(new ofApp);
	ofRunApp(main, mainApp);

	// visual
	s.setSize(1920, 1080);
	s.setPosition(glm::vec2(1921, 10));
	s.resizable = true;
	s.shareContextWith = main;
	shared_ptr<ofAppBaseWindow> visual = ofCreateWindow(s);
	shared_ptr<VisualApp> visApp(new VisualApp);
	visApp->score = &mainApp->score;
	ofRunApp(visual, visApp);

	ofRunMainLoop();

}
