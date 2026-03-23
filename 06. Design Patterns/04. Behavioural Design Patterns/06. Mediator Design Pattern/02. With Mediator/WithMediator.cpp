#include <iostream>
#include <vector>
#include <string>

using namespace std;

// forward‐declare Colleague so IMediator can refer to it
class Colleague;

// ─────────────── Mediator Interface ───────────────
class IMediator {
public:
    virtual ~IMediator() {}
    virtual void registerColleague(Colleague* c) = 0;
    virtual void send(const string& from, const string& msg) = 0;
    virtual void sendPrivate(const string& from, const string& to, const string& msg) = 0;
};

// ─────────────── Colleague Interface ───────────────
class Colleague {
protected:
    IMediator* mediator;  

public:
    Colleague(IMediator* m) {
        mediator = m;
        mediator->registerColleague(this);
    }
    virtual ~Colleague() {}

    virtual string getName() = 0;
    virtual void send(const string& msg) = 0;
    virtual void sendPrivate(const string& to, const string& msg) = 0;
    virtual void receive(const string& from, const string& msg) = 0;
};

// ─────────────── Concrete Mediator ───────────────
class ChatMediator : public IMediator {
private:
    vector<Colleague*> colleagues;
    vector<pair<string,string>> mutes; // (muter, muted)

public:
    void registerColleague(Colleague* c) override {
        colleagues.push_back(c);
    }

    void mute(const string& who, const string& whom)  {
        mutes.push_back(make_pair(who, whom));
    }

    void send(const string& from, const string& msg) override {
        cout << "[" << from << " broadcasts]: " << msg << "\n";
        for (Colleague* c : colleagues) {
            // Don't send msg to itself.
            if (c->getName() == from) continue;

            bool isMuted = false;
            // Ignore if person is muted
            for (auto& p : mutes) {
                if (from == p.second && c->getName() == p.first) {
                    isMuted = true;
                    break;
                }
            }
            if (!isMuted) {
                c->receive(from, msg);
            }
        }
    }

    void sendPrivate(const string& from, const string& to, const string& msg) override {
        cout << "[" << from << "→" << to << "]: " << msg << "\n";
        for (Colleague* c : colleagues) {
            if (c->getName() == to) {
                for (auto& p : mutes) {

                    //Dont send if muted
                    if (from == p.second && to == p.first) {
                        cout << "\n[Message is muted]\n";
                        return;
                    }
                }
                c->receive(from, msg);
                return;
            }
        }
        cout << "[Mediator] User \"" << to << "\" not found]\n";
    }
};

// ─────────────── Concrete Colleague ───────────────
class User : public Colleague {
private:
    string name;

public:
    User(const string& n, IMediator* m)
      : Colleague(m) {
        name = n;
    }

    string getName() override {
        return name;
    }

    void send(const string& msg) override {
        mediator->send(name, msg);
    }

    void sendPrivate(const string& to, const string& msg) override {
        mediator->sendPrivate(name, to, msg);
    }

    void receive(const string& from, const string& msg) override {
        cout << "    " << name << " got from " << from << ": " << msg << "\n";
    }
};

// ─────────────── Demo ───────────────
int main() {
    ChatMediator* chatRoom = new ChatMediator();

    User* user1 = new User("Rohan", chatRoom);
    User* user2 = new User("Neha",  chatRoom);
    User* user3 = new User("Mohan", chatRoom);

    // Rohan mutes Mohan
    chatRoom->mute("Rohan", "Mohan");

    // broadcast from Rohan
    user1->send("Hello Everyone!");

    // private from Mohan to Neha
    user3->sendPrivate("Neha", "Hey Neha!");

    // cleanup
    delete user1;
    delete user2;
    delete user3;
    delete chatRoom;
    return 0;
}