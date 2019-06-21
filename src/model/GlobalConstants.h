#pragma once

class ScoreManager;

static unsigned char& BEAT = ScoreManager::beat;
static unsigned char& BAR = ScoreManager::barCount;
static unsigned char& CHANNEL = ScoreManager::channelCount;
static unsigned char& PITCH = ScoreManager::pitchCount;
static unsigned char MAX_BAR = 4;