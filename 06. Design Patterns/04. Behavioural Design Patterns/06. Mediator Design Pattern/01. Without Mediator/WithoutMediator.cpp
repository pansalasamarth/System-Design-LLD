#include <iostream>
#include <vector>
#include <string>
using namespace std;

// Each User knows *all* the others directly.
// If you have N users, you wind up wiring N*(N–1)/2 connections,
// and every new feature (mute, private send, logging...) lives in User too.

class User {
private:
    string name;
    vector<User*> peers;
    vector<string> mutedUsers;

public:
    User(const string& n) {
        name = n;
    }

    // must manually connect every pair → N^2 wiring
    void addPeer(User* u) {
        peers.push_back(u);
    }

    // duplication: everyone has its own mute list
    void mute(const string& userToMute) {
        mutedUsers.push_back(userToMute);
    }

    // broadcast to all peers
    void send(const string& msg) {
        cout << "[" << name << " broadcasts]: " << msg << endl;
        for (User* peer : peers) {
            
            // if they have muted me dont send.
            if(!peer->isMuted(name)) {
                peer->receive(name, msg);
            }
        }
    }

    bool isMuted(string userName) {
        for(auto name : mutedUsers) {
            if(name == userName) {
                return true;
            }
        }
        return false;
    }

    // private send - duplicated in every class
    void sendTo(User* target, const string& msg) {
        cout << "[" << name << "→" << target->name << "]: " << msg << endl;
        if(!target->isMuted(name)) {
            target->receive(name, msg);
        }
    }

    void receive(const string& from, const string& msg) {
        cout << "    " << name << " got from " << from << ": " << msg << endl;
    }
};

int main() {
    // create users
    User* user1 = new User("Samarth");
    User* user2 = new User("Ansh");
    User* user3 = new User("Maulik");

    // wire up peers (each knows each other) → n*(n-1)/2 connections
    user1->addPeer(user2);   
    user2->addPeer(user1);

    user1->addPeer(user3);   
    user3->addPeer(user1);

    user2->addPeer(user3); 
    user3->addPeer(user2);

    // mute example: Mohan mutes Rohan (Hence Rohan add Mohan to its muted list).
    user1->mute("Maulik");

    // broadcast
    user1->send("Hello everyone!");

    // private
    user3->sendTo(user2, "Hey Ansh!");

    // cleanup
    delete user1;
    delete user2;
    delete user3;
    return 0;
}