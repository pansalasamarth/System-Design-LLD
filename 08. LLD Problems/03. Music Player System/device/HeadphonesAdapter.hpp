#pragma once
#include "../models/Song.hpp"
#include "IAudioOutputDevice.hpp"
#include "../external/HeadphonesAPI.hpp"

using namespace std;

class HeadphonesAdapter : public IAudioOutputDevice {
private:
    HeadphonesAPI* headphonesApi;
public:
    HeadphonesAdapter(HeadphonesAPI* api) {
        headphonesApi = api;
    }

    void playAudio(Song* song) override {
        string payload = song->getTitle() + " by " + song->getArtist();
        headphonesApi->playSoundViaJack(payload);
    }
};

