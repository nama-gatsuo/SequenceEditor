#pragma once
#include "ofMain.h"
#include "ofxMidi.h"

class MidiSender {
public:
	MidiSender() {
		//midiOut.listPorts();
		/// LoopMidi = 1
		midiOut.openPort(1);
	}

	~MidiSender() {
		midiOut.closePort();
	}

	void sendMidiOn(UCHAR ch, UCHAR midiNum, UCHAR velocity) {
		midiOut.sendNoteOn(ch, midiNum, velocity);
	}

	void sendMidiOff(UCHAR ch, UCHAR midiNum) {
		midiOut.sendNoteOff(ch, midiNum);
	}
	
private:
	ofxMidiOut midiOut;
};