#pragma once

class ScoreManager;

static UCHAR& BEAT = ScoreManager::beat;
static UCHAR& BAR = ScoreManager::barCount;
static UCHAR& CHANNEL = ScoreManager::channelCount;
static UCHAR& PITCH = ScoreManager::pitchCount;
static UCHAR MAX_BAR = 4;