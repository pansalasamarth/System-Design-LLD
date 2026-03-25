#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>

using namespace std;

// Forward declarations
class User;
class Group;
class ExpenseManager;

enum class SplitType {
    EQUAL,
    EXACT,
    PERCENTAGE
};

class Split {
public:
    string userId;
    double amount;
    
    Split(const string& userId, double amount) {
        this->userId = userId;
        this->amount = amount;
    }
};

// Observer Pattern - Notification interface
class Observer {
public:
    virtual void update(const string& message) = 0;
};

// Strategy Pattern - Split strategies
class SplitStrategy {
public:
    virtual vector<Split> calculateSplit(double totalAmount, const vector<string>& userIds, 
                                       const vector<double>& values = {}) = 0;
};

class EqualSplit : public SplitStrategy {
public:
    vector<Split> calculateSplit(double totalAmount, const vector<string>& userIds, 
                               const vector<double>& values = {}) override {
        vector<Split> splits;
        double amountPerUser = totalAmount / userIds.size();
        
        for (const string& userId : userIds) {
            splits.push_back(Split(userId, amountPerUser));
        }
        return splits;
    }
};

class ExactSplit : public SplitStrategy {
public:
    vector<Split> calculateSplit(double totalAmount, const vector<string>& userIds, 
                               const vector<double>& values = {}) override {
        vector<Split> splits;

        //validations
        
        for (int i = 0; i < userIds.size(); i++) {
            splits.push_back(Split(userIds[i], values[i]));
        }
        return splits;
    }
};

class PercentageSplit : public SplitStrategy {
public:
    vector<Split> calculateSplit(double totalAmount, const vector<string>& userIds, 
                               const vector<double>& values = {}) override {
        vector<Split> splits;

        //validations
        
        for (int i = 0; i < userIds.size(); i++) {
            double amount = (totalAmount * values[i]) / 100.0;
            splits.push_back(Split(userIds[i], amount));
        }
        return splits;
    }
};

// Factory for split strategies
class SplitFactory {
public:
    static SplitStrategy* getSplitStrategy(SplitType type) {
        switch (type) {
            case SplitType::EQUAL:
                return new EqualSplit();
            case SplitType::EXACT:
                return new ExactSplit();
            case SplitType::PERCENTAGE:
                return new PercentageSplit();
            default:
                return new EqualSplit();
        }
    }
};

// User class --> Concrete Observer
class User : public Observer {
public:
    static int nextUserId;
    string userId;
    string name;
    string email;
    map<string, double> balances; // userId -> amount (positive = they owe you, negative = you owe them)
    
    User(const string& name, const string& email) {
        this->userId = "user" + to_string(++nextUserId);
        this->name = name;
        this->email = email;
    }
    
    void update(const string& message) override {
        cout << "[NOTIFICATION to " << name << "]: " << message << endl;
    }
    
    void updateBalance(const string& otherUserId, double amount) {
        balances[otherUserId] += amount;
        
        // Remove if balance becomes zero
        if (abs(balances[otherUserId]) < 0.01) {
            balances.erase(otherUserId);
        }
    }
    
    double getTotalOwed() {
        double total = 0;
        for (const auto& balance : balances) {
            if (balance.second < 0) {
                total += abs(balance.second);
            }
        }
        return total;
    }
    
    double getTotalOwing() {
        double total = 0;
        for (const auto& balance : balances) {
            if (balance.second > 0) {
                total += balance.second;
            }
        }
        return total;
    }
};
int User::nextUserId = 0;

// Expense Model class
class Expense {
public:
    static int nextExpenseId;
    string expenseId;
    string description;
    double totalAmount;
    string paidByUserId;
    vector<Split> splits;
    string groupId;
    
    Expense(const string& desc, double amount, const string& paidBy,
            vector<Split>& splits, const string group="") {
        this->expenseId = "expense" + std::to_string(++nextExpenseId);
        this->description = desc;
        this->totalAmount = amount;
        this->paidByUserId = paidBy;
        this->splits = splits;
        this->groupId = group;
    }
};
int Expense::nextExpenseId = 0;

class DebtSimplifier {
public:
    static map<string, map<string, double>> simplifyDebts(
        map<string, map<string, double>> groupBalances) {
        
        // Calculate net amount for each person
        map<string, double> netAmounts;
        
        // Initialize all users with 0
        for (const auto& userBalance : groupBalances) {
            netAmounts[userBalance.first] = 0;
        }
        
        // Calculate net amounts
        // We only need to process each balance once (not twice)
        // If groupBalances[A][B] = 200, it means B owes A 200
        // So A should receive 200 (positive) and B should pay 200 (negative)
        for (const auto& userBalance : groupBalances) {
            string creditorId = userBalance.first;
            for (const auto& balance : userBalance.second) {
                string debtorId = balance.first;
                double amount = balance.second;
                
                // Only process positive amounts to avoid double counting
                if (amount > 0) {
                    netAmounts[creditorId] += amount;  // creditor receives
                    netAmounts[debtorId] -= amount;    // debtor pays
                }
            }
        }
        
        // Divide users into creditors and debtors
        vector<pair<string, double>> creditors; // those who should receive money
        vector<pair<string, double>> debtors;   // those who should pay money
        
        for (const auto& net : netAmounts) {
            if (net.second > 0.01) { // creditor
                creditors.push_back({net.first, net.second});
            } else if (net.second < -0.01) { // debtor
                debtors.push_back({net.first, -net.second}); // store positive amount
            }
        }
        
        // Sort for better optimization (largest amounts first)
        sort(creditors.begin(), creditors.end(), 
             [](const pair<string, double>& a, const pair<string, double>& b) {
                 return a.second > b.second;
             });
        sort(debtors.begin(), debtors.end(), 
             [](const pair<string, double>& a, const pair<string, double>& b) {
                 return a.second > b.second;
             });
        
        // Create new simplified balance map
        map<string, map<string, double>> simplifiedBalances;
        
        // Initialize empty maps for all users
        for (const auto& userBalance : groupBalances) {
            simplifiedBalances[userBalance.first] = map<string, double>();
        }
        
        // Use greedy algorithm to minimize transactions
        int i = 0, j = 0;
        while (i < creditors.size() && j < debtors.size()) {
            string creditorId = creditors[i].first;
            string debtorId = debtors[j].first;
            double creditorAmount = creditors[i].second;
            double debtorAmount = debtors[j].second;
            
            // Find the minimum amount to settle
            double settleAmount = min(creditorAmount, debtorAmount);
            
            // Update simplified balances
            // debtorId owes creditorId the settleAmount
            simplifiedBalances[creditorId][debtorId] = settleAmount;
            simplifiedBalances[debtorId][creditorId] = -settleAmount;
            
            // Update remaining amounts
            creditors[i].second -= settleAmount;
            debtors[j].second -= settleAmount;
            
            // Move to next creditor or debtor if current one is settled
            if (creditors[i].second < 0.01) {
                i++;
            }
            if (debtors[j].second < 0.01) {
                j++;
            }
        }
        
        return simplifiedBalances;
    }
};

// Group class --> Concrete Observable
class Group {
private:
    User* getUserByuserId(string userId) {
        User* user = nullptr;

        for(User *member : members) {
            if(member->userId == userId) {
                user = member;
            }
        }
        return user;
    }
    
public:
    static int nextGroupId;
    string groupId;
    string name;
    vector<User*> members; //observers
    map<string, Expense*> groupExpenses; // Group's own expense book
    map<string, map<string, double>> groupBalances; // memberId -> {otherMemberId -> balance}
    
    Group(const string& name) {
        this->groupId = "group" + std::to_string(++nextGroupId);
        this->name = name;
    }
    
    ~Group() {
        // Clean up group's expenses
        for (auto& pair : groupExpenses) {
            delete pair.second;
        }
    }
    
    void addMember(User* user) {
        members.push_back(user);

        // Initialize balance map for new member
        groupBalances[user->userId] = map<string, double>();
        cout << user->name << " added to group " << name << endl;
    }
    
    bool removeMember(const string& userId) {    
        // Check if user can be removed or not
        if(!canUserLeaveGroup(userId)) {
            cout <<"\nUser not allowed to leave group without clearing expenses" << endl;
            return false;
        }
        
        // Remove from observers
        for (User *user : members) {
            if (user->userId == userId) {
                members.erase(remove(members.begin(), members.end(), user),members.end());
                break;
            }
        }
        
        // Remove from group balances
        groupBalances.erase(userId);
        
        // Remove this user from other members' balance maps
        for (auto& memberBalance : groupBalances) {
            memberBalance.second.erase(userId);
        }
        return true;
    }
    
    void notifyMembers(const string& message) {
        for (Observer* observer : members) {
            observer->update(message);
        }
    }

    bool isMember(const string& userId) {
        return groupBalances.find(userId) != groupBalances.end();
    }
    
    // Update balance within group
    void updateGroupBalance(const string& fromUserId, const string& toUserId, double amount) {
        groupBalances[fromUserId][toUserId] += amount;
        groupBalances[toUserId][fromUserId] -= amount;
        
        // Remove if balance becomes zero
        if (abs(groupBalances[fromUserId][toUserId]) < 0.01) {
            groupBalances[fromUserId].erase(toUserId);
        }
        if (abs(groupBalances[toUserId][fromUserId]) < 0.01) {
            groupBalances[toUserId].erase(fromUserId);
        }
    }
    
    // Check if user can leave group.
    bool canUserLeaveGroup(const string& userId) {
        if (!isMember(userId)) {
            throw runtime_error("user is not a part of this group");
        };
        
        // Check if user has any outstanding balance with other group members
        map<string, double> userBalanceSheet = groupBalances[userId];
        for (auto& balance : userBalanceSheet) {
            if (abs(balance.second) > 0.01) {
                return false; // Has outstanding balance
            }
        }
        return true;
    }
    
    // Get user's balance within this group
    map<string, double> getUserGroupBalances(const string& userId) {
        if (!isMember(userId)) {
            throw runtime_error("user is not a part of this group");
        };
        return groupBalances[userId];
    }
    
    // Add expense to this group
    bool addExpense(string& description, double amount, string& paidByUserId,
                   vector<string>& involvedUsers, SplitType splitType, 
                   const vector<double>& splitValues = {}) {
        
        if (!isMember(paidByUserId)) {
            throw runtime_error("user is not a part of this group");
        }
        
        // Validate that all involved users are group members
        for (const string& userId : involvedUsers) {
            if (!isMember(userId)) {
                throw runtime_error("involvedUsers are not a part of this group");
            }
        }
        
        // Generate splits using strategy pattern
        vector<Split> splits = SplitFactory::getSplitStrategy(splitType)
                                ->calculateSplit(amount, involvedUsers, splitValues);
        
        // Create expense in group's own expense book
        Expense* expense = new Expense(description, amount, paidByUserId, splits, groupId);
        groupExpenses[expense->expenseId] = expense;
        
        // Update group balances
        for (Split& split : splits) {
            if (split.userId != paidByUserId) {
                // Person who paid gets positive balance, person who owes gets negative
                updateGroupBalance(paidByUserId, split.userId, split.amount);
            }
        }
        
        cout << endl << "=========== Sending Notifications ===================="<<endl;
        string paidByName = getUserByuserId(paidByUserId)->name;
        notifyMembers("New expense added: " + description + " (Rs " + to_string(amount) + ")");
        
        // Printing console message-------
        cout << endl << "=========== Expense Message ===================="<<endl;
        cout << "Expense added to " << name << ": " << description << " (Rs " << amount 
             << ") paid by " << paidByName <<" and involved people are : "<< endl;
        if(!splitValues.empty()) {
            for(int i=0; i<splitValues.size(); i++) {
                cout << getUserByuserId(involvedUsers[i])->name << " : " << splitValues[i] << endl;
            }
        } 
        else {
            for(string user : involvedUsers) {
                cout << getUserByuserId(user)->name << ", ";
            }
            cout << endl << "Will be Paid Equally" << endl;
        }    
        //-----------------------------------
        
        return true;
    }
    
    bool settlePayment(string& fromUserId, string& toUserId, double amount) {
        // Validate that both users are group members
        if (!isMember(fromUserId) || !isMember(toUserId)) {
            cout << "user is not a part of this group" << endl;
            return false;
        }
        
        // Update group balances
        updateGroupBalance(fromUserId, toUserId, amount);
        
        // Get user names for display
        string fromName = getUserByuserId(fromUserId)->name;
        string toName = getUserByuserId(toUserId)->name;
        
        // Notify group members
        notifyMembers("Settlement: " + fromName + " paid " + toName + " Rs " + to_string(amount));
        
        cout << "Settlement in " << name << ": " << fromName << " settled Rs " 
             << amount << " with " << toName << endl;
        
        return true;
    }
    
    void showGroupBalances() {
        cout << "\n=== Group Balances for " << name << " ===" << endl;
        
        for (const auto& pair : groupBalances) {
            string memberId = pair.first;
            string memberName = getUserByuserId(memberId)->name;

            cout << memberName << "'s balances in group:" << endl;
            
            auto userBalances = pair.second;
            if (userBalances.empty()) {
                cout << "  No outstanding balances" << endl;
            } 
            else {
                for (const auto& userBalance : userBalances) {
                    string otherMemberUserId = userBalance.first;
                    string otherName = getUserByuserId(otherMemberUserId)->name;
                    
                    double balance = userBalance.second;
                    if (balance > 0) {
                        cout << "  " << otherName << " owes: Rs " << fixed << setprecision(2) << balance << endl;
                    } else {
                        cout << "  Owes " << otherName << ": Rs " << fixed << setprecision(2) << abs(balance) << endl;
                    }
                }
            }
        }
    }

    void simplifyGroupDebts() {
        map<string, map<string, double>> simplifiedBalances = DebtSimplifier::simplifyDebts(groupBalances);
        groupBalances = simplifiedBalances;
    
        cout << "\nDebts have been simplified for group: " << name << endl;
    }
};
int Group::nextGroupId = 0;
    
// Main ExpenseManager class (Singleton - Facade)
class Tricount {
private:
    map<string, User*> users;
    map<string, Group*> groups;
    map<string, Expense*> expenses;

    static Tricount* instance;
    Tricount() {}
    
public:
    static Tricount* getInstance() {
        if(instance == nullptr) {
            instance = new Tricount();
        }
        return instance;
    }

    // User management
    User* createUser(string name, string email) {
        User* user = new User(name, email);
        users[user->userId] = user;
        cout << "User created: " << name << " (ID: " << user->userId << ")" << endl;
        return user;
    }
    
    User* getUser(const string& userId) {
        auto it = users.find(userId);
        return (it != users.end()) ? it->second : nullptr;
    }
    
    // Group management
    Group* createGroup(const string name) {
        Group* group = new Group(name);
        groups[group->groupId] = group;
        cout << "Group created: " << name << " (ID: " << group->groupId << ")" << endl;
        return group;
    }
    
    Group* getGroup(const string& groupId) {
        auto it = groups.find(groupId);
        return (it != groups.end()) ? it->second : nullptr;
    }
    
    void addUserToGroup(const string& userId, const string& groupId) {
        User* user = getUser(userId);
        Group* group = getGroup(groupId);
        
        if (user && group) {
            group->addMember(user);
        }
    }
    
    // Try to remove user from group - just delegates to group
    bool removeUserFromGroup(const string& userId, const string& groupId) {
        Group* group = getGroup(groupId);
        
        if (!group) {
            cout << "Group not found!" << endl;
            return false;
        }
        
        User* user = getUser(userId);
        if (!user) {
            cout << "User not found!" << endl;
            return false;
        }

        bool userRemoved = group->removeMember(userId);
        
        if(userRemoved) {
            cout << user->name << " successfully left " << group->name << endl;
        }
        return userRemoved;
    }
    
    // Expense management - delegate to group
    void addExpenseToGroup(string& groupId, string description, double amount, 
                          string& paidByUserId, vector<string>& involvedUsers, 
                          SplitType splitType, const vector<double>& splitValues = {}) {
        
        Group* group = getGroup(groupId);
        if (!group) {
            cout << "Group not found!" << endl;
            return;
        }
        
        group->addExpense(description, amount, paidByUserId, involvedUsers, splitType, splitValues);
    }
    
    // Settlement - delegate to group
    void settlePaymentInGroup(string& groupId, string& fromUserId, 
                              string& toUserId, double amount) {
        
        Group* group = getGroup(groupId);
        if (!group) {
            cout << "Group not found!" << endl;
            return;
        }
        
        group->settlePayment(fromUserId, toUserId, amount);
    }
    
    // Settlement
    void settleIndividualPayment(string& fromUserId, string& toUserId, double amount) {
        User* fromUser = getUser(fromUserId);
        User* toUser = getUser(toUserId);
        
        if (fromUser && toUser) {
            fromUser->updateBalance(toUserId, amount);
            toUser->updateBalance(fromUserId, -amount);
            
            cout << fromUser->name << " settled Rs" << amount << " with " << toUser->name << endl;
        }
    }
    
    void addIndividualExpense(string description, double amount, string paidByUserId,
                             string toUserId, SplitType splitType,
                            const vector<double>& splitValues = {}) {

        SplitStrategy* strategy = SplitFactory::getSplitStrategy(splitType);
        vector<Split> splits = strategy->calculateSplit(amount, {paidByUserId, toUserId}, splitValues);

        Expense* expense = new Expense(description, amount, paidByUserId, splits);
        expenses[expense->expenseId] = expense;
        
        User* paidByUser = getUser(paidByUserId);
        User* toUser = getUser(toUserId);

        paidByUser->updateBalance(toUserId, amount);
        toUser->updateBalance(paidByUserId, -amount);
        
        cout << "Individual expense added: " << description << " (Rs " << amount 
                << ") paid by " << paidByUser->name <<" for " << toUser->name << endl;
    }
    
    // Display Method
    void showUserBalance(string& userId) {
        User* user = getUser(userId);
        if (!user) return;
        
        cout << endl << "=========== Balance for " << user->name <<" ===================="<<endl; 
        cout << "Total you owe: Rs " << fixed << setprecision(2) << user->getTotalOwed() << endl;
        cout << "Total others owe you: Rs " << fixed << setprecision(2) << user->getTotalOwing() << endl;
        
        cout << "Detailed balances:" << endl;
        for (auto& balance : user->balances) {
            User* otherUser = getUser(balance.first);
            if (otherUser) {
                if (balance.second > 0) {
                    cout << "  " << otherUser->name << " owes you: Rs" << balance.second << endl;
                } else {
                    cout << "  You owe " << otherUser->name << ": Rs" << abs(balance.second) << endl;
                }
            }
        }
    }
    
    void showGroupBalances(string& groupId) {
        Group* group = getGroup(groupId);
        if (!group) return;
        
        group->showGroupBalances();
    }
    
    void simplifyGroupDebts(string& groupId) {
        Group* group = getGroup(groupId);
        if (!group) return;
                
        // Use group's balance data for debt simplification
        group->simplifyGroupDebts();
    }
};

Tricount* Tricount::instance = nullptr;

int main() {
    
    Tricount* manager = Tricount::getInstance();
    
    cout << endl << "=========== Creating Users ===================="<<endl;
    User* user1 = manager->createUser("Samarth", "samarth@gmail.com");
    User* user2 = manager->createUser("Ansh", "ansh@gmail.com");
    User* user3 = manager->createUser("Maulik", "maulik@gmail.com");
    User* user4 = manager->createUser("Shivansh", "shivansh@gmail.com");
    
    cout << endl << "=========== Creating Group and Adding Members ===================="<<endl;
    Group* hostelGroup = manager->createGroup("Hostel Expenses");
    manager->addUserToGroup(user1->userId, hostelGroup->groupId);
    manager->addUserToGroup(user2->userId, hostelGroup->groupId);
    manager->addUserToGroup(user3->userId, hostelGroup->groupId);
    manager->addUserToGroup(user4->userId, hostelGroup->groupId);

    cout << endl << "=========== Adding Expenses in group ===================="<<endl;    
    vector<string> groupMembers = {user1->userId, user2->userId, user3->userId, user4->userId};
    manager->addExpenseToGroup(hostelGroup->groupId, "Lunch", 800.0, user1->userId, groupMembers, SplitType::EQUAL);
    
    vector<string> dinnerMembers = {user1->userId, user3->userId, user4->userId};
    vector<double> dinnerAmounts = {200.0, 300.0, 200.0};
    manager->addExpenseToGroup(hostelGroup->groupId, "Dinner", 700.0, user3->userId, dinnerMembers, 
                             SplitType::EXACT, dinnerAmounts);

    cout << endl << "=========== printing Group-Specific Balances ===================="<<endl; 
    manager->showGroupBalances(hostelGroup->groupId);

    cout << endl << "=========== Debt Simplification ===================="<<endl; 
    manager->simplifyGroupDebts(hostelGroup->groupId);

    cout << endl << "=========== printing Group-Specific Balances ===================="<<endl; 
    manager->showGroupBalances(hostelGroup->groupId);

    cout << endl << "=========== Adding Individual Expense ===================="<<endl; 
    manager->addIndividualExpense("Coffee", 40.0, user2->userId, user4->userId, SplitType::EQUAL);
    
    cout << endl << "=========== printing User Balances ===================="<<endl; 
    manager->showUserBalance(user1->userId);
    manager->showUserBalance(user2->userId);
    manager->showUserBalance(user3->userId);
    manager->showUserBalance(user4->userId);

    cout << endl << "==========Attempting to remove Rohit from group==========" << endl;
    manager->removeUserFromGroup(user2->userId, hostelGroup->groupId);

    cout << endl << "======== Making Settlement to Clear Rohit's Debt =========="<<endl; 
    manager->settlePaymentInGroup(hostelGroup->groupId, user2->userId, user3->userId, 200.0);
    
    cout << endl << "======== Attempting to Remove Rohit Again =========="<<endl;
    manager->removeUserFromGroup(user2->userId, hostelGroup->groupId);
    
    cout << endl << "=========== Updated Group Balances ===================="<<endl; 
    manager->showGroupBalances(hostelGroup->groupId);
    
    return 0;
}