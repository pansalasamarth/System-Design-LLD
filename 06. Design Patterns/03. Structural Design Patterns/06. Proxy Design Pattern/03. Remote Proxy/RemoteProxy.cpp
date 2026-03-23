#include<iostream>
#include<string>

using namespace std;

class IDataService {
public:
    virtual string fetchData() = 0;
    virtual ~IDataService() = default;
};

class RealDataService : public IDataService {
public:
    RealDataService() {
        // Imagine this connects to a remote server or loads heavy resources.
        cout << "[RealDataService] Initialized (simulating remote setup)\n";
    }
    string fetchData() override {
        return "[RealDataService] Data from server";
    }
};

// Remote proxy
class DataServiceProxy : public IDataService {
private:
    RealDataService* realService = nullptr;

public:
    DataServiceProxy() {
        realService = new RealDataService();
    }

    string fetchData() override {
        cout << "[DataServiceProxy] Connecting to remote service...\n";
        return realService->fetchData();
    }
};

int main() {
    IDataService* dataService = new DataServiceProxy();
    dataService->fetchData();
}