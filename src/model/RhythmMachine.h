#pragma once
#include "ofMain.h"
#include "Events.h"

class RhythmMachine : public ofThread {
public:
	RhythmMachine() : delta(125) {}
	~RhythmMachine() {
		stop();
		waitForThread(false);
	}
	void setDelta(int delta) {

		if (lock()) {
			this->delta = delta;
			unlock();
		} else {
			ofLogWarning("BpmCounter::setBpm()") << "Unable to lock mutex.";
		}

	}
	
	void threadedFunction() {

		while (isThreadRunning()) {
			if (lock()) {
				
				unlock();
				sleep(delta);
				bool isNew = true;

				ofNotifyEvent(EventsEntity::newBeat, isNew);
			} else {
				ofLogWarning("BpmCounter::threadedFunction()") << "Unable to lock mutex.";
			}
		}
	}

	void stop() {
		std::unique_lock<std::mutex> lck(mutex);
		stopThread();
		condition.notify_all();
	}

private:
	std::condition_variable condition;
	int delta;
};