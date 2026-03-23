#pragma once
#include "../models/Song.hpp"

class IAudioOutputDevice {
public:
    virtual ~IAudioOutputDevice() {}
    virtual void playAudio(Song* song) = 0;
};