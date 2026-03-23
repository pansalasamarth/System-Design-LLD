#include <iostream>
#include <string>

using namespace std;

// Interface for Document Reader
class IDocumentReader {
public:
    virtual void unlockPDF(string filePath, string password) = 0;
    virtual ~IDocumentReader() = default;
};

// Concrete Class: Reads the PDF (simulated)
class RealDocumentReader : public IDocumentReader {
public:
    void unlockPDF(string filePath, string password) override {
        cout << "[RealDocumentReader] Unlocking PDF at: " << filePath << "\n";
        cout << "[RealDocumentReader] PDF unlocked successfully with password: " << password << "\n";
        cout << "[RealDocumentReader] Displaying PDF content...\n";
    }
};

// User class with membership status
class User {
public:
    string name;
    bool premiumMembership;
    
    User(string name, bool isPremium) {
        this->name = name;
        this->premiumMembership = isPremium;
    }
};

// Proxy Class: Controls access to RealDocumentReader
class DocumentProxy : public IDocumentReader {
    RealDocumentReader* realReader;
    User* user;
    
public:
    DocumentProxy(User* user) {
        realReader = new RealDocumentReader();
        this->user = user;
    }

    void unlockPDF(string filePath, string password) override {
        if (!user->premiumMembership) {
            cout << "[DocumentProxy] Access denied. Only premium members can unlock PDFs.\n";
            return;
        }

        // Forwarding the request to the real reader
        realReader->unlockPDF(filePath, password);
    }

    ~DocumentProxy() {
        delete realReader;
    }
};

// Client code
int main() {

    User* user1 = new User("Rohan", false);  // Non Premium User
    User* user2 = new User("Rashmi", true);  // premium user

    cout << "== Rohan (Non-Premium) tries to unlock PDF ==\n";
    IDocumentReader* docReader = new DocumentProxy(user1);
    docReader->unlockPDF("protected_document.pdf", "secret123");
    delete docReader;

    cout << "\n== Rashmi (Premium) unlocks PDF ==\n";
    docReader = new DocumentProxy(user2);
    docReader->unlockPDF("protected_document.pdf", "secret123");
    delete docReader;

    return 0;
}