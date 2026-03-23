#include <iostream>
using namespace std;

// Abstract Handler
class LeaveHandler {
protected:
    LeaveHandler* nextHandler;

public:
    LeaveHandler() {
        nextHandler = nullptr;
    }

    void setNextHandler(LeaveHandler* next) {
        nextHandler = next;
    }

    virtual void approveLeave(int days) = 0;
};

// Team Leader Handler
class TeamLeader : public LeaveHandler {
public:
    void approveLeave(int days) override {
        if(days <= 2) {
            cout << "Team Leader approved " << days << " days leave.\n";
        } 
        else if(nextHandler != nullptr) {
            cout << "Team Leader forwarded request to Manager.\n";
            nextHandler->approveLeave(days);
        }
    }
};

// Manager Handler
class Manager : public LeaveHandler {
public:
    void approveLeave(int days) override {
        if(days <= 5) {
            cout << "Manager approved " << days << " days leave.\n";
        } 
        else if(nextHandler != nullptr) {
            cout << "Manager forwarded request to Director.\n";
            nextHandler->approveLeave(days);
        }
    }
};

// Director Handler
class Director : public LeaveHandler {
public:
    void approveLeave(int days) override {
        cout << "Director approved " << days << " days leave.\n";
    }
};

// Client
int main() {

    LeaveHandler* teamLeader = new TeamLeader();
    LeaveHandler* manager = new Manager();
    LeaveHandler* director = new Director();

    // Create chain
    teamLeader->setNextHandler(manager);
    manager->setNextHandler(director);

    int leaveDays = 10;

    cout << "Leave Request: " << leaveDays << " days\n";

    teamLeader->approveLeave(leaveDays);

    return 0;
}