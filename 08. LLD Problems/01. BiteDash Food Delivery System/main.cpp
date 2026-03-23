#include <iostream>
#include "BiteDashApp.h"
using namespace std;

int main() {
    // Create BiteDashApp Object
    BiteDashApp* BiteDash = new BiteDashApp();

    // Simulate a user coming in (Happy Flow)
    User* user = new User(101, "Samarth", "Surat");
    cout << "User: " << user->getName() << " is active." << endl;

    // User searches for restaurants by location
    vector<Restaurant*> restaurantList = BiteDash->searchRestaurants("Surat");

    if (restaurantList.empty()) {
        cout << "No restaurants found!" << endl;
        return 0;
    }
    cout << "Found Restaurants:" << endl;
    for (auto restaurant : restaurantList) {
        cout << " - " << restaurant->getName() << endl;
    }

    // User selects a restaurant
    BiteDash->selectRestaurant(user, restaurantList[0]);

    cout << "Selected restaurant: " << restaurantList[0]->getName() << endl;

    // User adds items to the cart
    BiteDash->addToCart(user, "P1");
    BiteDash->addToCart(user, "P2");

    BiteDash->printUserCart(user);

    // User checkout the cart
    Order* order = BiteDash->checkoutNow(user, "Delivery", new UpiPaymentStrategy("1234567890"));
    
    // User pay for the cart. If payment is success, notification is sent.
    BiteDash->payForOrder(user, order);

    // Cleanup Code.
    delete BiteDash;
    delete user;
    
    return 0;
}
