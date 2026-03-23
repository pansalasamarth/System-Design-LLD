#include <iostream>
#include <vector>

using namespace std;

class Product {
public:
    string name;
    double price;

    Product(string name, double price) {
        this->name = name;
        this->price = price;
    }
};

class ShoppingCart {
private:
    vector<Product*> products;

public:
    void addProduct(Product* p) {
        products.push_back(p);
    }

    const vector<Product*>& getProducts() const {
        return products;
    }

    double calculateTotal() const {
        double total = 0;
        for (auto p : products) {
            total += p->price;
        }
        return total;
    }
};

// Responsible only for printing invoice
class InvoicePrinter {
public:
    void printInvoice(const ShoppingCart& cart) {
        cout << "Shopping Cart Invoice:\n";

        for (auto p : cart.getProducts()) {
            cout << p->name << " - Rs " << p->price << endl;
        }

        cout << "Total: Rs " << cart.calculateTotal() << endl;
    }
};

// Responsible only for database operations
class CartRepository {
public:
    void save(const ShoppingCart& cart) {
        cout << "Saving shopping cart to database..." << endl;
    }
};

int main() {

    ShoppingCart cart;

    cart.addProduct(new Product("Laptop", 50000));
    cart.addProduct(new Product("Mouse", 2000));

    InvoicePrinter printer;
    printer.printInvoice(cart);

    CartRepository repo;
    repo.save(cart);

    return 0;
}