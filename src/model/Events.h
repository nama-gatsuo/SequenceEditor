#pragma once
#include "ofMain.h"

struct ExecRandom {
	ExecRandom(int bar, int ch) : bar(bar), ch(ch) {}
	int bar;
	int ch;
};

struct EventsEntity {
	static ofEvent<bool> newBeat;
	static ofEvent<int> editTarget;
	static ofEvent<ExecRandom> execRandom;
};

