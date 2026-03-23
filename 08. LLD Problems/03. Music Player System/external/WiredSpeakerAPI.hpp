#pragma once
#include<string>
#include<iostream>

using namespace std;

class WiredSpeakerAPI {
public:
    void playSoundViaCable(const string& data) {
        cout << "[WiredSpeaker] Playing: " << data << "\n";
        // mimics playing music
    }
};