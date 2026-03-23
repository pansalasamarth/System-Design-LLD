#pragma once
#include<string>
#include<iostream>

using namespace std;

class HeadphonesAPI {
public:
    void playSoundViaJack(const string& data) {
        cout << "[Headphones] Playing: " << data << "\n";
        // mimics playing music
    }
};