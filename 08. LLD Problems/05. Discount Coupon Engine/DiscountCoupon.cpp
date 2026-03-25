#include <vector>
#include <string>
#include <mutex>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

// ----------------------------
// Discount Strategy (Strategy Pattern)
// ----------------------------
class DiscountStrategy {
public:
    virtual ~DiscountStrategy() {}
    virtual double calculate(double baseAmount) = 0;
};

class FlatDiscountStrategy : public DiscountStrategy {
private:
    double amount;
public:
    FlatDiscountStrategy(double amt) {
        amount = amt;
    }
    double calculate(double baseAmount) override {
        return min(amount, baseAmount);
    }
};

class PercentageDiscountStrategy : public DiscountStrategy {
private:
    double percent;
public:
    PercentageDiscountStrategy(double pct) {
        percent = pct;
    }
    double calculate(double baseAmount) override {
        return (percent / 100.0) * baseAmount;
    }
};

class PercentageWithCapStrategy : public DiscountStrategy {
private:
    double percent;
    double cap;
public:
    PercentageWithCapStrategy(double pct, double capVal) {
        percent = pct;
        cap = capVal;
    }
    double calculate(double baseAmount) override {
        double disc = (percent / 100.0) * baseAmount;
        if (disc > cap) {
            return cap;
        }
        return disc;
    }
};

enum StrategyType {
    FLAT,
    PERCENT,
    PERCENT_WITH_CAP
};

// ----------------------------
// DiscountStrategyManager (Singleton)
// ----------------------------
class DiscountStrategyManager {
private:
    static DiscountStrategyManager* instance;
    DiscountStrategyManager() {}
    DiscountStrategyManager(const DiscountStrategyManager&) = delete;
    DiscountStrategyManager& operator=(const DiscountStrategyManager&) = delete;
public:
    static DiscountStrategyManager* getInstance() {
        if (!instance) {
            instance = new DiscountStrategyManager();
        }
        return instance;
    }
    DiscountStrategy* getStrategy(StrategyType type, double param1, double param2 = 0.0) const {
        if (type == StrategyType::FLAT) {
            return new FlatDiscountStrategy(param1);
        }
        if (type == StrategyType::PERCENT) {
            return new PercentageDiscountStrategy(param1);
        }
        if (type == StrategyType::PERCENT_WITH_CAP) {
            return new PercentageWithCapStrategy(param1, param2);
        }
        return nullptr;
    }
};
// Initialize static member
DiscountStrategyManager* DiscountStrategyManager::instance = nullptr;

// ----------------------------
// Assume existing Cart and Product classes
// ----------------------------
class Product {
private:
    string name;
    string category;
    double price;
public:
    Product(string name, string category, double price) {
        this->name = name;
        this->category = category;
        this->price = price;
    }
    string getName() {
        return name;
    }
    string getCategory() const {
        return category;
    }
    double getPrice() {
        return price;
    }
};

class CartItem {
private:
    Product* product;
    int quantity;
public:
    CartItem(Product* prod, int qty) {
        product = prod;
        quantity = qty;
    }
    double itemTotal() {
        return product->getPrice() * quantity;
    }
    const Product* getProduct() {
        return product;
    }
};

class Cart {
private:
    vector<CartItem*> items;
    double originalTotal;
    double currentTotal;
    bool loyaltyMember;
    string paymentBank;
public:
    Cart() {
        originalTotal = 0.0;
        currentTotal = 0.0;
        loyaltyMember = false;
        paymentBank = "";
    }

    void addProduct(Product* prod, int qty = 1) {
        CartItem* item = new CartItem(prod, qty);
        items.push_back(item);
        originalTotal += item->itemTotal();
        currentTotal  += item->itemTotal();
    }

    double getOriginalTotal() {
        return originalTotal;
    }

    double getCurrentTotal() {
        return currentTotal;
    }

    void applyDiscount(double d) {
        currentTotal -= d;
        if (currentTotal < 0) {
            currentTotal = 0;
        }
    }

    void setLoyaltyMember(bool member) {
        loyaltyMember = member;
    }

    bool isLoyaltyMember() {
        return loyaltyMember;
    }

    void setPaymentBank(string bank) {
        paymentBank = bank;
    }

    string getPaymentBank() {
        return paymentBank;
    }

    vector<CartItem*> getItems() {
        return items;
    }
};

// ----------------------------
// Coupon base class (Chain of Responsibility)
// ----------------------------
class Coupon {
private:
    Coupon* next;
public:
    Coupon() {
        next = nullptr;
    }
    virtual ~Coupon() {
        if (next) {
            delete next;
        }
    }
    void setNext(Coupon* nxt) {
        next = nxt;
    }
    Coupon* getNext() {
        return next;
    }
    
    void applyDiscount(Cart* cart) {
        if (isApplicable(cart)) {
            double discount = getDiscount(cart);
            cart->applyDiscount(discount);
            cout << name() << " applied: " << discount << endl;
            if (!isCombinable()) {
                return;
            }
        }
        if (next) {
            next->applyDiscount(cart);
        }
    }
    virtual bool isApplicable(Cart* cart) = 0;
    virtual double getDiscount(Cart* cart) = 0;
    virtual bool isCombinable() {
        return true;
    }
    virtual string name() = 0;
};

// ----------------------------
// Concrete Coupons
// ----------------------------
class SeasonalOffer : public Coupon {
private:
    double percent;
    string category;
    DiscountStrategy* strat;
public:
    SeasonalOffer(double pct, string cat) {
        percent = pct;
        category = cat;
        strat = DiscountStrategyManager::getInstance()->getStrategy(StrategyType::PERCENT, percent);
    }
    ~SeasonalOffer() {
        delete strat;
    }
    bool isApplicable(Cart* cart) override {
        for (CartItem* item : cart->getItems()) {
            if (item->getProduct()->getCategory() == category) {
                return true;
            }
        }
        return false;
    }
    double getDiscount(Cart* cart) override {
        double subtotal = 0.0;
        for (CartItem* item : cart->getItems()) {
            if (item->getProduct()->getCategory() == category) {
                subtotal += item->itemTotal();
            }
        }
        return strat->calculate(subtotal);
    }
    bool isCombinable() override {
        return true;
    }
    string name() override {
        return "Seasonal Offer " + to_string((int)percent) + " % off " + category;
    }
};

class LoyaltyDiscount : public Coupon {
private:
    double percent;
    DiscountStrategy* strat;
public:
    LoyaltyDiscount(double pct) {
        percent = pct;
        strat = DiscountStrategyManager::getInstance()->getStrategy(StrategyType::PERCENT, percent);
    }
    ~LoyaltyDiscount() {
        delete strat;
    }
    bool isApplicable(Cart* cart) override {
        return cart->isLoyaltyMember();
    }
    double getDiscount(Cart* cart) override {
        return strat->calculate(cart->getCurrentTotal());
    }
    string name() override {
        return "Loyalty Discount " + to_string((int)percent) + "% off";
    }
};

class BulkPurchaseDiscount : public Coupon {
private:
    double threshold;
    double flatOff;
    DiscountStrategy* strat;
public:
    BulkPurchaseDiscount(double thr, double off) {
        threshold = thr;
        flatOff = off;
        strat = DiscountStrategyManager::getInstance()->getStrategy(StrategyType::FLAT, flatOff);
    }
    ~BulkPurchaseDiscount() {
        delete strat;
    }
    bool isApplicable(Cart* cart) override {
        return cart->getOriginalTotal() >= threshold;
    }
    double getDiscount(Cart* cart) override {
        return strat->calculate(cart->getCurrentTotal());
    }
    string name() override {
        return "Bulk Purchase Rs " + to_string((int)flatOff) + " off over "
             + to_string((int)threshold);
    }
};

class BankingCoupon : public Coupon {
private:
    string bank;
    double minSpend;
    double percent;
    double offCap;
    DiscountStrategy* strat;
public:
    BankingCoupon(const string& b, double ms, double percent, double offCap) {
        bank = b;
        minSpend = ms;
        this->percent = percent;
        this->offCap = percent;
        strat = DiscountStrategyManager::getInstance()->getStrategy(StrategyType::PERCENT_WITH_CAP, percent, offCap);
    }
    ~BankingCoupon() {
        delete strat;
    }
    bool isApplicable(Cart* cart) override {
        return cart->getPaymentBank() == bank
            && cart->getOriginalTotal() >= minSpend;
    }
    double getDiscount(Cart* cart) override {
        return strat->calculate(cart->getCurrentTotal());
    }
    string name() override {
        return bank + " Bank Rs " + to_string((int)percent) + " off upto " + to_string((int) offCap);
    }
};

// ----------------------------
// CouponManager (Singleton)
// ----------------------------
class CouponManager {
private:
    static CouponManager* instance;
    Coupon* head;
    mutable mutex mtx;
    CouponManager() {
        head = nullptr;
    }
public:
    static CouponManager* getInstance() {
        if (!instance) {
            instance = new CouponManager();
        }
        return instance;
    }

    void registerCoupon(Coupon* coupon) {
        lock_guard<mutex> lock(mtx);
        if (!head) {
            head = coupon;
        } else {
            Coupon* cur = head;
            while (cur->getNext()) {
                cur = cur->getNext();
            }
            cur->setNext(coupon);
        }
    }

    vector<string> getApplicable(Cart* cart) const {
        lock_guard<mutex> lock(mtx);
        vector<string> res;
        Coupon* cur = head;
        while (cur) {
            if (cur->isApplicable(cart)) {
                res.push_back(cur->name());
            }
            cur = cur->getNext();
        }
        return res;
    }

    double applyAll(Cart* cart) {
        lock_guard<mutex> lock(mtx);
        if (head) {
            head->applyDiscount(cart);
        }
        return cart->getCurrentTotal();
    }
};
// Initialize static instance pointer
CouponManager* CouponManager::instance = nullptr;

// ----------------------------
// Main: Client code (heap allocations and pointers)
// ----------------------------
int main() {

    CouponManager* mgr = CouponManager::getInstance();
    mgr->registerCoupon(new SeasonalOffer(10, "Clothing"));
    mgr->registerCoupon(new LoyaltyDiscount(5));
    mgr->registerCoupon(new BulkPurchaseDiscount(1000, 100));
    mgr->registerCoupon(new BankingCoupon("ABC", 2000, 15, 500));

    Product* p1 = new Product("Winter Jacket", "Clothing", 1000);
    Product* p2 = new Product("Smartphone", "Electronics", 20000);
    Product* p3 = new Product("Jeans", "Clothing", 1000);
    Product* p4 = new Product("Headphones", "Electronics", 2000);

    Cart* cart = new Cart();
    cart->addProduct(p1, 1);
    cart->addProduct(p2, 1);
    cart->addProduct(p3, 2);
    cart->addProduct(p4, 1);
    cart->setLoyaltyMember(true);
    cart->setPaymentBank("ABC");

    cout << "Original Cart Total: " << cart->getOriginalTotal() << " Rs" << endl;



    vector<string> applicable = mgr->getApplicable(cart);
    cout << "Applicable Coupons:" << endl;
    for (string name : applicable) {
        cout << " - " << name << endl;
    }

    double finalTotal = mgr->applyAll(cart);
    cout << "Final Cart Total after discounts: " << finalTotal << " Rs" << endl;

    // Cleanup code
    delete p1;
    delete p2;
    delete p3;
    delete p4;
    delete cart;

    return 0;
}
