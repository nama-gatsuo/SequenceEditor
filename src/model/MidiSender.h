#pragma once
#include "ofxMidi.h"

class MidiSender {
public:
	MidiSender() {
		midiOut.listOutPorts();
		/// LoopMidi = 1
		midiOut.openPort(1);
	}

	~MidiSender() {
		midiOut.closePort();
	}

	void sendMidiOn(int ch, int midiNum, int velocity) {
		midiOut.sendNoteOn(ch, midiNum, velocity);
	}

	void sendMidiOff(int ch, int midiNum) {
		midiOut.sendNoteOff(ch, midiNum);
	}
	
private:
	ofxMidiOut midiOut;
};