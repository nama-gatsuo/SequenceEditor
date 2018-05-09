#pragma once
#include "ofMain.h"
#include "ofxMidi.h"

class MidiSender {
public:
	MidiSender() {
		midiOut.listPorts();
		// LoopMidi = 1
		midiOut.openPort(1);
	}

	~MidiSender() {
		midiOut.closePort();
	}

	void sendMidiOn(USHORT ch, USHORT midiNum, USHORT velocity) {
		midiOut.sendNoteOn(ch, midiNum, velocity);
	}

	void sendMidiOff(USHORT ch, USHORT midiNum) {
		midiOut.sendNoteOff(ch, midiNum);
	}
	
private:
	ofxMidiOut midiOut;
};