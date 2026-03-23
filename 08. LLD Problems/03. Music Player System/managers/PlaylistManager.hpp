#pragma once
#include "../models/Playlist.hpp"
#include<iostream>
#include<string>
#include<map>

using namespace std;

class PlaylistManager {
private:
    static PlaylistManager* instance;
    map<string, Playlist*> playlists;
    PlaylistManager() {}
public:
    static PlaylistManager* getInstance() {
        if (!instance) {
            instance = new PlaylistManager();
        }
        return instance;
    }

    void createPlaylist(const string& name) {
        if (playlists.count(name)) {
            throw runtime_error("Playlist \"" + name + "\" already exists.");
        }
        playlists[name] = new Playlist(name);
    }

    void addSongToPlaylist(const string& playlistName, Song* song) {
        if (!playlists.count(playlistName)) {
            throw runtime_error("Playlist \"" + playlistName + "\" not found.");
        }  
        playlists[playlistName]->addSongToPlaylist(song);
    }

    Playlist* getPlaylist(const string& name) {
        if (!playlists.count(name)) {
            throw runtime_error("Playlist \"" + name + "\" not found.");
        }
        return playlists[name];
    }
};
PlaylistManager* PlaylistManager::instance = nullptr;