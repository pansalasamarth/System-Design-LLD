#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>

using namespace std;

/////////////////////////////////////////////
// Product & Factory
/////////////////////////////////////////////

class Product {
private:
    int sku;
    string name;
    double price;

public:
    Product(int id, string nm, double pr) {
        sku   = id;
        name  = nm;
        price = pr;
    }

    //Getters & Setters
    int getSku() {
        return this->sku;
    }

    string getName() {
        return this->name;
    }
    double getPrice() {
        return this->price;
    }
};

class ProductFactory {
public:
    static Product* createProduct(int sku) {
        // In reality product comes from DB
        string name;
        double price;

        if (sku == 101) {
            name  = "Apple";
            price = 20;
        }
        else if (sku == 102) {
            name  = "Banana";
            price = 10;
        }
        else if (sku == 103) {
            name  = "Chocolate";
            price = 50;
        }
        else if (sku == 201) {
            name  = "Pizza";
            price = 500;
        }
        else if (sku == 202) {
            name  = "Burger";
            price = 1000;
        }
        else {
            name  = "Item" + to_string(sku);
            price = 100;
        }
        return new Product(sku, name, price);
    }
};

/////////////////////////////////////////////
// InventoryStore (Abstract) & DbInventoryStore
/////////////////////////////////////////////

class InventoryStore {
public:
    virtual ~InventoryStore() {}
    virtual void addProduct(Product* prod, int qty) = 0;
    virtual void removeProduct(int sku, int qty) = 0;
    virtual int checkStock(int sku) = 0;
    virtual vector<Product*> listAvailableProducts() = 0;
};

class DbInventoryStore : public InventoryStore {
private:
    map<int,int>* stock;             // SKU -> quantity
    map<int,Product*>* products;     // SKU -> Product*
public:
    DbInventoryStore() {
        stock = new map<int,int>();
        products = new map<int,Product*>();
    }
    ~DbInventoryStore() {
        for (auto it : *products) {
            delete it.second;
        }
        delete products;
        delete stock;
    }

    void addProduct(Product* prod, int qty) override {
        int sku = prod->getSku();
        if (products->count(sku) == 0) {
            // Does not exist
            (*products)[sku] = prod;
        } else {
            delete prod;
        }
        (*stock)[sku] += qty;
    }

    void removeProduct(int sku, int qty) override {
        if (stock->count(sku) == 0) 
            return;

        int currentQuantity = (*stock)[sku];
        int remainingQuantity = currentQuantity - qty;
        if (remainingQuantity > 0) {
            (*stock)[sku] = remainingQuantity;
        } else {
            stock->erase(sku);
        }
    }

    int checkStock(int sku) override {
        if (stock->count(sku) == 0) 
            return 0;

        return (*stock)[sku];
    }

    vector<Product*> listAvailableProducts() override {
        vector<Product*> available;
        for (auto it : *stock) {
            int sku = it.first;
            int qty = it.second;
            if (qty > 0 && products->count(sku)) {
                available.push_back((*products)[sku]);
            }
        }
        return available;
    }
};

/////////////////////////////////////////////
// InventoryManager 
/////////////////////////////////////////////

class InventoryManager {
    InventoryStore* store;

public:
    InventoryManager(InventoryStore* store) {
        this->store = store;
    }

    void addStock(int sku, int qty) {
        Product* prod = ProductFactory::createProduct(sku);
        store->addProduct(prod, qty);
        cout << "[InventoryManager] Added SKU " << sku << " Qty " << qty << endl;
    }

    void removeStock(int sku, int qty) {
        store->removeProduct(sku, qty); 
    }

    int checkStock(int sku) {
        return store->checkStock(sku);
    }

    vector<Product*> getAvailableProducts() {
        return store->listAvailableProducts();
    }
};

/////////////////////////////////////////////
// Replenishment Strategy (Strategy Pattern)
/////////////////////////////////////////////

class ReplenishStrategy {
public:
    virtual void replenish(InventoryManager* manager, map<int,int> itemsToReplenish) = 0;
    virtual ~ReplenishStrategy() {}
};

class ThresholdReplenishStrategy : public ReplenishStrategy {
private:
    int threshold;
public:
    ThresholdReplenishStrategy(int threshold) {
        this->threshold = threshold;
    }
    void replenish(InventoryManager* manager, map<int,int> itemsToReplenish) override {
        cout << "[ThresholdReplenish] Checking threshold... \n";
        for (auto it : itemsToReplenish) {
            int sku = it.first;
            int qtyToAdd = it.second;
            int current  = manager->checkStock(sku);
            if (current < threshold) {
                manager->addStock(sku, qtyToAdd);
                cout << "  -> SKU " << sku << " was " << current 
                     << ", replenished by " << qtyToAdd << endl;
            }
        }
    }
};

class WeeklyReplenishStrategy : public ReplenishStrategy {
public:
    WeeklyReplenishStrategy() {}
    void replenish(InventoryManager* manager, map<int,int> itemsToReplenish) override {
        cout << "[WeeklyReplenish] Weekly replenishment triggered for inventory.\n";
    }
};

/////////////////////////////////////////////
// DarkStore (formerly Warehouse)
/////////////////////////////////////////////

class DarkStore {
private:
    string name;
    double x, y;                       // location coordinates
    InventoryManager* inventoryManager;
    ReplenishStrategy* replenishStrategy;

public:
    DarkStore(string n, double x_coord, double y_coord) {
        name = n;
        x = x_coord;
        y = y_coord;
        
        // We could have made another factory called InventoryStoreFactory to get
        // DbInventoryStore by enum and hence make it loosely coupled.
        inventoryManager = new InventoryManager(new DbInventoryStore);

    }
    ~DarkStore() {
        delete inventoryManager;
        if (replenishStrategy) delete replenishStrategy;
    }

    double distanceTo(double ux, double uy) {
        return sqrt((x - ux)*(x - ux) + (y - uy)*(y - uy));
    }

    void runReplenishment(map<int,int> itemsToReplenish) {
        if (replenishStrategy) {
            replenishStrategy->replenish(inventoryManager, itemsToReplenish);
        }
    }

    // Delegation Methods
    vector<Product*> getAllProducts() {
        return inventoryManager->getAvailableProducts();
    }

    int checkStock(int sku) {
        return inventoryManager->checkStock(sku);
    }

    void removeStock(int sku, int qty) {
        inventoryManager->removeStock(sku, qty); 
    }

    void addStock(int sku, int qty) {
        Product* prod = ProductFactory::createProduct(sku);
        inventoryManager->addStock(sku, qty);
    }

    // Getters & Setters
    void setReplenishStrategy(ReplenishStrategy* strategy) {
        this->replenishStrategy = strategy;
    }

    string getName() {
        return this->name;
    }

    double getXCoordinate() {
        return this->x;
    }

    double getYCoordinate() {
        return this->y;
    }

    InventoryManager* getInventoryManager() {
        return this->inventoryManager;
    }
};

/////////////////////////////////////////////
// DarkStoreManager (Singleton)
/////////////////////////////////////////////

class DarkStoreManager {
private:
    vector<DarkStore*>* darkStores;
    static DarkStoreManager* instance;

    DarkStoreManager() {
        darkStores = new vector<DarkStore*>();
    }

public:
    static DarkStoreManager* getInstance() {
        if(instance == nullptr) {
            instance = new DarkStoreManager();
        }
        return instance;
    }

    void registerDarkStore(DarkStore* ds) {
        darkStores->push_back(ds);
    }

    vector<DarkStore*> getNearbyDarkStores(double ux, double uy, double maxDistance) {
        vector<pair<double,DarkStore*>> distList;
        for (auto ds : *darkStores) {
            double d = ds->distanceTo(ux, uy);
            if (d <= maxDistance) {
                distList.push_back(make_pair(d, ds));
            }
        }
        sort(distList.begin(), distList.end(),
             [](auto &a, auto &b){ return a.first < b.first; });

        vector<DarkStore*> result;
        for (auto &p : distList) {
            result.push_back(p.second);
        }
        return result;
    }

    ~DarkStoreManager() {
        for (auto ds : *darkStores) {
            delete ds;
        }
        delete darkStores;
    }
};

DarkStoreManager* DarkStoreManager::instance = nullptr;

/////////////////////////////////////////////
// User & Cart
/////////////////////////////////////////////

class Cart {
public:
    vector<pair<Product*,int>> items;  // (Product*, qty)

    void addItem(int sku, int qty) {
        Product* prod = ProductFactory::createProduct(sku);
        items.push_back(make_pair(prod, qty));
        cout << "[Cart] Added SKU " << sku << " (" << prod->getName() 
             << ") x" << qty << endl;
    }

    double getTotal() {
        double sum = 0.0;
        for (auto &it : items) {
            sum += (it.first->getPrice() * it.second);
        }
        return sum;
    }

    vector<pair<Product*,int>> getItems() {
        return items;
    }

    ~Cart() {
        for (auto &it : items) {
            delete it.first;
        }
    }
};

class User {
public:
    string name;
    double x, y;
    Cart* cart;  // User owns a cart

    User(string n, double x_coord, double y_coord) {
        name = n;
        x = x_coord;
        y = y_coord;
        cart = new Cart();
    }
    ~User() {
        delete cart;
    }

    Cart* getCart() {
        return cart;
    }
};

/////////////////////////////////////////////
// DeliveryPartner
/////////////////////////////////////////////

class DeliveryPartner {
public:
    string name;
    DeliveryPartner(string n) {
        name = n;
    }
};

/////////////////////////////////////////////
// Order & OrderManager (Singleton)
/////////////////////////////////////////////

class Order {
public:
    static int nextId;
    int orderId;
    User* user;
    vector<pair<Product*,int>> items;     // (Product*, qty)
    vector<DeliveryPartner*> partners;
    double totalAmount;

    Order(User* u) {
        orderId = nextId++;
        user = u;
        totalAmount = 0.0;
    }
};

int Order::nextId = 1;

// Singleton
class OrderManager {
private:
    vector<Order*>* orders;
    static OrderManager* instance;

    OrderManager() {
        orders = new vector<Order*>();
    }

public:
    static OrderManager* getInstance() {
        if(instance == nullptr) {
            instance = new OrderManager();
        }
        return instance;
    }

    void placeOrder(User* user, Cart* cart) {
        cout << "\n[OrderManager] Placing Order for: " << user->name << "\n";

        // product --> Qty
        vector<pair<Product*,int>> requestedItems = cart->getItems();
    
        // 1) Find nearby dark stores within 5 KM
        double maxDist = 5.0;
        vector<DarkStore*> nearbyDarkStores = DarkStoreManager::getInstance()->getNearbyDarkStores(user->x, user->y, maxDist);
        
        if (nearbyDarkStores.empty()) {
            cout << "  No dark stores within 5 KM. Cannot fulfill order.\n";
            return;
        }
    
        // 2) Check if closest store has all items
        DarkStore* firstStore = nearbyDarkStores.front();

        bool allInFirst = true;
        for (pair<Product*,int>& item : requestedItems) {

            int sku = item.first->getSku();
            int qty = item.second;

            if (firstStore->checkStock(sku) < qty) {
                allInFirst = false;
                break;
            }
        }
    
        Order* order = new Order(user);

        // One delivery partner required...
        if (allInFirst) {

            cout << "  All items at: " << firstStore->getName() << "\n";

            // Remove the products from store
            for (pair<Product*,int>& item : requestedItems) {
                int sku = item.first->getSku();
                int qty = item.second;
                firstStore->removeStock(sku, qty);
                order->items.push_back({ item.first, qty });
            }

            order->totalAmount = cart->getTotal();
            order->partners.push_back(new DeliveryPartner("Partner1"));
            cout << "  Assigned Delivery Partner: Partner1\n";
        } 

        // Multiple delivery partners required
        else {
            cout << "  Splitting order across stores...\n";

            map<int,int> allItems; //SKU --> Qty

            for (pair<Product*,int>& item : requestedItems) {
                allItems[item.first->getSku()] = item.second;
            }
    
            int partnerId = 1;
            for (DarkStore* store : nearbyDarkStores) {

                // If allItems becomes empty, we break early (all SKUs have been assigned)
                if (allItems.empty()) break;

                cout << "   Checking: " << store->getName() << "\n";
    
                bool assigned = false;
                vector<int> toErase;
                
                for (auto& [sku, qtyNeeded] : allItems) {

                    int availableQty = store->checkStock(sku);
                    if (availableQty <= 0) continue;
    
                    //take whichever is smaller: available or qtyNeeded.
                    int takenQty = min(availableQty, qtyNeeded);
                    store->removeStock(sku, takenQty);

                    cout << "     " << store->getName() << " supplies SKU " << sku 
                         << " x" << takenQty << "\n";

                    order->items.push_back({ ProductFactory::createProduct(sku), takenQty });
    
                    // Adjust the Quantity
                    if (qtyNeeded > takenQty) {
                        allItems[sku] = qtyNeeded - takenQty;
                    } else {
                        toErase.push_back(sku);
                    }
                    assigned = true;
                }

                // After iterating all SKUs in allItems, we erase 
                // any fully‐satisfied SKUs from the allItems map.
                for (int sku : toErase) allItems.erase(sku);
    
                // If at least one SKU was taken from this store, we assign 
                // a new DeliveryPartner.
                if (assigned) {
                    string pname = "Partner" + to_string(partnerId++);
                    order->partners.push_back(new DeliveryPartner(pname));
                    cout << "     Assigned: " << pname << " for " << store->getName() << "\n";
                }
            }
    
            //  if remaining still has entries, we print which SKUs/quantities could not be fulfilled.
            if (!allItems.empty()) {
                cout << "  Could not fulfill:\n";
                for (auto& [sku, qty] : allItems) {
                    cout << "    SKU " << sku << " x" << qty << "\n";
                }
            }

            // recompute order->totalAmount
            double sum = 0;
            for (auto& item : order->items) {
                sum += item.first->getPrice() * item.second;
            }
            order->totalAmount = sum;
        }
    
        // Printing Order Summary
        cout << "\n[OrderManager] Order #" << order->orderId << " Summary:\n";
        cout << "  User: " << user->name << "\n  Items:\n";
        for (auto& item : order->items) {
            cout << "    SKU " << item.first->getSku()
                 << " (" << item.first->getName() << ") x" << item.second
                 << " @ ₹" << item.first->getPrice() << "\n";
        }
        cout << "  Total: ₹" << order->totalAmount << "\n  Partners:\n";
        for (auto* dp : order->partners) {
            cout << "    " << dp->name << "\n";
        }
        cout << endl;
    
        orders->push_back(order);

        // Cleanups
        for (auto* dp : order->partners) delete dp;
        for (auto& item : order->items) delete item.first;
    }    

    vector<Order*> getAllOrders() {
        return *orders;
    }

    ~OrderManager() {
        for (auto ord : *orders) {
            delete ord;
        }
        delete orders;
    }
};

OrderManager* OrderManager::instance = nullptr;

/////////////////////////////////////////////
// Main(): High-Level Flow
/////////////////////////////////////////////
class SwiggyHelper {
public:
    static void showAllItems(User* user) {
        cout << "\n[Swiggy] All Available products within 5 KM for " << user->name << ":\n";

        DarkStoreManager* dsManager = DarkStoreManager::getInstance();
        vector<DarkStore*> nearbyStores = dsManager->getNearbyDarkStores(user->x, user->y, 5.0);

        // Collect each SKU → price (so we only display each product once)
        map<int, double> skuToPrice;
        map<int, string> skuToName;

        for (DarkStore* darkStore : nearbyStores) {
            vector<Product*> products = darkStore->getAllProducts();

            for (Product* product : products) {
                int sku = product->getSku();

                if (skuToPrice.count(sku) == 0) {
                    skuToPrice[sku] = product->getPrice();
                    skuToName[sku]  = product->getName();
                }
            }
        }

        for (auto& entry : skuToPrice) {
            int sku = entry.first;
            double price = entry.second;
            cout << "  SKU " << sku << " - " << skuToName[sku] << " @ ₹" << price << "\n";
        }
    }

    static void initialize() {
        auto dsManager = DarkStoreManager::getInstance();

        // DarkStore A.......
        DarkStore* darkStoreA = new DarkStore("DarkStoreA", 0.0, 0.0);
        darkStoreA->setReplenishStrategy(new ThresholdReplenishStrategy(3));
 
        cout << "\nAdding stocks in DarkStoreA...." << endl;  
        darkStoreA->addStock(101, 5); // Apple
        darkStoreA->addStock(102, 2); // Banana

        // DarkStore B.......
        DarkStore* darkStoreB = new DarkStore("DarkStoreB", 4.0, 1.0);
        darkStoreB->setReplenishStrategy(new ThresholdReplenishStrategy(3));

        cout << "\nAdding stocks in DarkStoreB...." << endl; 
        darkStoreB->addStock(101, 3); // Apple
        darkStoreB->addStock(103, 10); // Chocolate

        // DarkStore C.......
        DarkStore* darkStoreC = new DarkStore("DarkStoreC", 2.0, 3.0);
        darkStoreC->setReplenishStrategy(new ThresholdReplenishStrategy(3));

        cout << "\nAdding stocks in DarkStoreC...." << endl; 
        darkStoreC->addStock(102, 5); // Banana
        darkStoreC->addStock(201, 7); // Pizza

        dsManager->registerDarkStore(darkStoreA);
        dsManager->registerDarkStore(darkStoreB);
        dsManager->registerDarkStore(darkStoreC);
    }
};

/////////////////////////////////////////////
// Main(): High-Level Flow
/////////////////////////////////////////////

int main() {

    // 1) Initialize.
    SwiggyHelper::initialize();

    // 2) A User comes on Platform
    User* user = new User("Samarth", 1.0, 1.0);
    cout <<"\nUser with name " << user->name<< " comes on platform" << endl;

    // 3) Show all available items via Swiggy
    SwiggyHelper::showAllItems(user);

    // 4) User adds items to cart (some not in a single store)
    cout<<"\nAdding items to cart\n";
    Cart* cart = user->getCart();
    cart->addItem(101, 4);  // dsA has 5, dsB has 3 
    cart->addItem(102, 3);  // dsA has 2, dsC has 5
    cart->addItem(103, 2);  // dsB has 10

    // 5) Place Order
    OrderManager::getInstance()->placeOrder(user, user->cart);

    // 6) Cleanup
    delete user;
    delete DarkStoreManager::getInstance();  // deletes all DarkStores and their inventoryManagers

    return 0;
}
