#include "Events.h"

ofEvent<bool> EventsEntity::newBeat;
ofEvent<int> EventsEntity::editTarget;
ofEvent<ExecRandom> EventsEntity::execRandom;