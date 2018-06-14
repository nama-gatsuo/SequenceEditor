#pragma once
#include "ofMain.h"

struct EventsEntity {
	static ofEvent<bool> newBeat;
	static ofEvent<int> editTarget;
};

