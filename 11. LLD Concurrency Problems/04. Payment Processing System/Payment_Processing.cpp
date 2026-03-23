#include <iostream>
#include <unordered_map>
#include <string>
#include <pthread.h>
#include <unistd.h>

using namespace std;

// ---------------- TRANSACTION ----------------
enum Status { PENDING, SUCCESS, FAILED };

struct Transaction {
    string txnId;
    int userId;
    int amount;
    Status status;
};

// ---------------- ACCOUNT ----------------
class Account {
public:
    int userId;
    int balance;
    pthread_mutex_t mutex;

    Account(int id, int bal) {
        userId = id;
        balance = bal;
        pthread_mutex_init(&mutex, NULL);
    }

    ~Account() {
        pthread_mutex_destroy(&mutex);
    }
};

// ---------------- TRANSACTION MANAGER ----------------
class TransactionManager {
private:
    unordered_map<string, Transaction> transactions;
    pthread_mutex_t mutex;

public:
    TransactionManager() {
        pthread_mutex_init(&mutex, NULL);
    }

    bool exists(string txnId) {
        pthread_mutex_lock(&mutex);
        bool found = transactions.find(txnId) != transactions.end();
        pthread_mutex_unlock(&mutex);
        return found;
    }

    void save(Transaction txn) {
        pthread_mutex_lock(&mutex);
        transactions[txn.txnId] = txn;
        pthread_mutex_unlock(&mutex);
    }

    void updateStatus(string txnId, Status status) {
        pthread_mutex_lock(&mutex);
        transactions[txnId].status = status;
        pthread_mutex_unlock(&mutex);
    }
};

// ---------------- PAYMENT SERVICE ----------------
class PaymentService {
private:
    unordered_map<int, Account*> accounts;
    TransactionManager txnManager;

public:
    void addAccount(int userId, int balance) {
        accounts[userId] = new Account(userId, balance);
    }

    bool processPayment(string txnId, int userId, int amount) {

        // Idempotency check
        if(txnManager.exists(txnId)) {
            cout << "Duplicate transaction ignored: " << txnId << endl;
            return false;
        }

        Transaction txn = {txnId, userId, amount, PENDING};
        txnManager.save(txn);

        Account* acc = accounts[userId];

        pthread_mutex_lock(&acc->mutex);

        if(acc->balance < amount) {
            txnManager.updateStatus(txnId, FAILED);
            pthread_mutex_unlock(&acc->mutex);
            cout << "Transaction Failed: Insufficient Balance\n";
            return false;
        }

        // Deduct balance (CRITICAL SECTION)
        acc->balance -= amount;

        txnManager.updateStatus(txnId, SUCCESS);

        pthread_mutex_unlock(&acc->mutex);

        cout << "Transaction Success: " << txnId 
             << " Remaining Balance: " << acc->balance << endl;

        return true;
    }
};

// ---------------- THREAD ROUTINE ----------------
struct ThreadArg {
    PaymentService* service;
    string txnId;
    int userId;
    int amount;
};

void* makePayment(void* arg) {
    ThreadArg* t = (ThreadArg*)arg;

    t->service->processPayment(t->txnId, t->userId, t->amount);

    delete t;
    return NULL;
}

// ---------------- MAIN ----------------
int main() {
    PaymentService service;

    service.addAccount(1, 1000);

    pthread_t t[3];

    for(int i = 0; i < 3; i++) {
        ThreadArg* arg = new ThreadArg{
            &service,
            "txn_" + to_string(i),
            1,
            400
        };

        pthread_create(&t[i], NULL, makePayment, arg);
    }

    for(int i = 0; i < 3; i++) {
        pthread_join(t[i], NULL);
    }

    return 0;
}