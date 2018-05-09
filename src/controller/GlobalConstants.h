#pragma once
#include "ofMain.h"
#include "ScoreManager.h"

static USHORT& BEAT = ScoreManager::beat;
static USHORT& BAR = ScoreManager::barCount;
static USHORT& CHANNEL = ScoreManager::channelCount;
static USHORT& PITCH = ScoreManager::pitchCount;