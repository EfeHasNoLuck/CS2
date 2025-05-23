#include <iostream>
#include <string>
#include <vector>
#include <iomanip>  // For std::fixed and std::setprecision
#include <limits>   // For std::numeric_limits
#include <fstream>  // For file handling
#include <sstream>  // For std::ostringstream
#include <algorithm>

class Product {
public:
    // Default constructor
    Product() : name(""), basePrice(0.0), csfloatPrice(0.0), bynogamePrice(0.0), specialValue(0.0) {}

    // Parameterized constructor
    Product(const std::string& name, double basePrice, double specialValue = 0.0)
        : name(name), basePrice(basePrice), csfloatPrice(0.0), bynogamePrice(0.0), specialValue(specialValue) {}

    // Method to calculate selling prices based on multipliers
    void calculateSellingPrices(double csfloatMultiplier, double bynogameMultiplier, double dollarRate) {
        csfloatPrice = basePrice * csfloatMultiplier;
        bynogamePrice = basePrice * bynogameMultiplier * dollarRate;
    }

    // Method to display the product details and prices
    void displayPrices() const {
        std::cout << "Product: " << name << std::endl;
        std::cout << "Base Price: $" << std::fixed << std::setprecision(3) << basePrice << std::endl;
        std::cout << "Special Value: " << std::fixed << std::setprecision(5) << specialValue << std::endl;
        std::cout << "Selling price for CSFloat: $" << csfloatPrice << std::endl;
        std::cout << "Selling price for ByNoGame: " << bynogamePrice << "TL" << std::endl;
    }

    // Accessor for base price
    double getBasePrice() const { return basePrice; }

    // Accessor for product name
    std::string getName() const { return name; }

    // Accessor for special value
    double getSpecialValue() const { return specialValue; }

    // Method to get a unique key for the product (combination of name and special value)
    std::string getUniqueKey() const {
        std::ostringstream oss;
        oss << name << "_" << std::fixed << std::setprecision(7) << specialValue;
        return oss.str();
    }

    // Serialize product data to a file
    void saveToFile(std::ofstream& outFile) const {
        outFile << name << "\n" << basePrice << "\n" << specialValue << "\n";
    }

    // Deserialize product data from a file
    static Product loadFromFile(std::ifstream& inFile) {
        std::string name;
        double basePrice, specialValue;
        std::getline(inFile, name);
        inFile >> basePrice >> specialValue;
        inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Ignore the newline after the special value
        return Product(name, basePrice, specialValue);
    }

private:
    std::string name;          // Product name
    double basePrice;          // Base price of the product
    double csfloatPrice;       // Selling price for CSFloat
    double bynogamePrice;      // Selling price for ByNoGame
    double specialValue;       // Special float value between 0 and 1
};

// Global variable to store the dollar rate
double dollarRate = 1.0;

// Function to save the entire product catalog to a file
void saveCatalogToFile(const std::vector<Product>& productCatalog) {
    std::ofstream outFile("product_catalog.txt");
    if (!outFile) {
        std::cerr << "Error opening file for writing." << std::endl;
        return;
    }
    for (const auto& product : productCatalog) {
        product.saveToFile(outFile);
    }
}

// Function to load the product catalog from a file
void loadCatalogFromFile(std::vector<Product>& productCatalog) {
    std::ifstream inFile("product_catalog.txt");
    if (!inFile) {
        std::cerr << "Error opening file for reading or file does not exist." << std::endl;
        return;
    }
    while (inFile.peek() != EOF) {
        Product product = Product::loadFromFile(inFile);
        productCatalog.push_back(product);
    }
}

// Function to save the dollar rate to a file
void saveDollarRate() {
    std::ofstream outFile("dollar_rate.txt");
    if (!outFile) {
        std::cerr << "Error opening file for writing the dollar rate." << std::endl;
        return;
    }
    outFile << dollarRate << std::endl;
}

// Function to load the dollar rate from a file
void loadDollarRate() {
    std::ifstream inFile("dollar_rate.txt");
    if (!inFile) {
        std::cerr << "Error opening file for reading the dollar rate. Using default rate of 1.0." << std::endl;
        dollarRate = 1.0;
        return;
    }
    inFile >> dollarRate;
}

// Function to add a product to the catalog
// Function to add a product to the catalog
void addProduct(std::vector<Product>& productCatalog) {
    std::string name;
    double basePrice, specialValue;

    std::cout << "Enter the product name: ";
    std::getline(std::cin >> std::ws, name);  // `std::ws` to ignore leading whitespace

    std::cout << "Enter the base price: ";
    std::cin >> basePrice;

    std::cout << "Enter the special float value (0.0 to 1.0): ";
    std::cin >> specialValue;

    // Validate specialValue
    if (specialValue < 0.0 || specialValue > 1.0) {
        std::cerr << "Invalid special value. It should be between 0.0 and 1.0." << std::endl;
        return;
    }

    // Format specialValue to a fixed precision of 4 decimal places for consistent comparison
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4) << specialValue;
    std::string formattedSpecialValue = oss.str();

    // Check for duplicates based on name and formatted special value
    auto it = std::find_if(productCatalog.begin(), productCatalog.end(), [&](const Product& product) {
        std::ostringstream existingValue;
        existingValue << std::fixed << std::setprecision(4) << product.getSpecialValue();  // Format existing special value too
        return product.getName() == name && existingValue.str() == formattedSpecialValue;
    });

    if (it != productCatalog.end()) {
        std::cerr << "Product with the same name and special float value already exists." << std::endl;
        return;
    }

    // Add the new product to the catalog
    Product newProduct(name, basePrice, specialValue);
    productCatalog.push_back(newProduct);
    saveCatalogToFile(productCatalog);  // Save the catalog after adding a product

    std::cout << "Product added successfully." << std::endl;
}

// Function to remove a product from the catalog
void removeProduct(std::vector<Product>& productCatalog) {
    std::string name;
    std::cout << "Enter the product name to remove: ";
    std::getline(std::cin >> std::ws, name);

    // Find matching products by name
    std::vector<Product> matchingProducts;
    for (const auto& product : productCatalog) {
        if (product.getName() == name) {
            matchingProducts.push_back(product);
        }
    }

    if (matchingProducts.empty()) {
        std::cerr << "Product not found." << std::endl;
        return;
    }

    if (matchingProducts.size() == 1) {
        // Only one matching product, remove it
        auto it = std::remove_if(productCatalog.begin(), productCatalog.end(), [&](const Product& product) {
            return product.getUniqueKey() == matchingProducts.front().getUniqueKey();
        });
        if (it != productCatalog.end()) {
            productCatalog.erase(it, productCatalog.end());
            saveCatalogToFile(productCatalog);  // Save the catalog after removing a product
            std::cout << "Product removed successfully." << std::endl;
        } else {
            std::cerr << "Failed to remove the product." << std::endl;
        }
    } else {
        // Multiple products found, prompt the user to select one
        std::cout << "Multiple products found. Please select one to remove:" << std::endl;
        for (size_t i = 0; i < matchingProducts.size(); ++i) {
            const auto& product = matchingProducts[i];
            std::cout << i + 1 << ": " << product.getName() << " | Special Value: "
                      << std::fixed << std::setprecision(4) << product.getSpecialValue() << std::endl;
        }

        size_t choice;
        std::cout << "Enter the number of the product to remove: ";
        std::cin >> choice;

        if (choice < 1 || choice > matchingProducts.size()) {
            std::cerr << "Invalid choice." << std::endl;
            return;
        }

        Product selectedProduct = matchingProducts[choice - 1];
        auto it = std::remove_if(productCatalog.begin(), productCatalog.end(), [&](const Product& product) {
            return product.getUniqueKey() == selectedProduct.getUniqueKey();
        });

        if (it != productCatalog.end()) {
            productCatalog.erase(it, productCatalog.end());
            saveCatalogToFile(productCatalog);  // Save the catalog after removing a product
            std::cout << "Product removed successfully." << std::endl;
        } else {
            std::cerr << "Failed to remove the product." << std::endl;
        }
    }
}

void calculateTotalBasePrice(const std::vector<Product>& productCatalog) {
    double totalBasePrice = 0.0;
    size_t productCount = productCatalog.size();  // Get the count of products

    for (const auto& product : productCatalog) {
        totalBasePrice += product.getBasePrice();
    }

    std::cout << "Total base price of all products: $" << std::fixed << std::setprecision(4) << totalBasePrice << std::endl;
    std::cout << "Total number of products: " << productCount << std::endl;
}

// Function to change the dollar rate
void changeDollarRate() {
    std::cout << "Enter the new dollar rate: ";
    std::cin >> dollarRate;

    // Validate dollar rate
    if (dollarRate <= 0) {
        std::cerr << "Invalid dollar rate. It should be greater than 0." << std::endl;
        return;
    }

    saveDollarRate();  // Save the new dollar rate to the file
    std::cout << "Dollar rate updated successfully." << std::endl;
}

// Function to sell a product
// Function to sell a product
void sellProduct(const std::vector<Product>& productCatalog) {
    std::string name;
    std::cout << "Enter the product name: ";
    std::getline(std::cin >> std::ws, name);

    std::vector<Product> matchingProducts;
    for (const auto& product : productCatalog) {
        if (product.getName() == name) {
            matchingProducts.push_back(product);
        }
    }

    if (matchingProducts.empty()) {
        std::cerr << "Product not found." << std::endl;
        return;
    }

    if (matchingProducts.size() == 1) {
        // Only one matching product, use it
        Product p = matchingProducts.front();
        const double csfloatMultiplier = 1.13;
        const double bynogameMultiplier = 1.23;

        // Calculate and display selling prices
        p.calculateSellingPrices(csfloatMultiplier, bynogameMultiplier, dollarRate);
        p.displayPrices();
    } else {
        // Multiple products found, prompt the user to select one
        std::cout << "Multiple products found. Please select one:" << std::endl;
        for (size_t i = 0; i < matchingProducts.size(); ++i) {
            const auto& product = matchingProducts[i];
            std::cout << i + 1 << ": " << product.getName() << " | Special Value: " 
                      << std::fixed << std::setprecision(4) << product.getSpecialValue() << std::endl;
        }

        size_t choice;
        std::cout << "Enter the number of the product to sell: ";
        std::cin >> choice;

        if (choice < 1 || choice > matchingProducts.size()) {
            std::cerr << "Invalid choice." << std::endl;
            return;
        }

        Product p = matchingProducts[choice - 1];
        const double csfloatMultiplier = 1.13;
        const double bynogameMultiplier = 1.23;

        // Calculate and display selling prices
        p.calculateSellingPrices(csfloatMultiplier, bynogameMultiplier, dollarRate);
        p.displayPrices();
    }
}

// Function to list all products
void listProducts(const std::vector<Product>& productCatalog) {
    if (productCatalog.empty()) {
        std::cout << "No products in the catalog." << std::endl;
        return;
    }

    std::cout << "Product Catalog:" << std::endl;
    for (const auto& product : productCatalog) {
        std::cout << "Product: " << product.getName() << ", Base Price: $" << std::fixed << std::setprecision(2) << product.getBasePrice() << ", Special Value: " << std::fixed << std::setprecision(4) << product.getSpecialValue() << std::endl;
    }
}

int main() {
    std::vector<Product> productCatalog;
    loadCatalogFromFile(productCatalog);  // Load the catalog at the beginning
    loadDollarRate();  // Load the dollar rate at the beginning

    char choice;
    do {
        std::cout << "Menu:\n";
        std::cout << "A - Add a product\n";
        std::cout << "S - Sell a product\n";
        std::cout << "L - List all products\n";
        std::cout << "R - Remove a product\n";
        std::cout << "T - Calculate total base price\n";
        std::cout << "D - Change dollar rate\n";
        std::cout << "Q - Quit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 'A':
            case 'a':
                addProduct(productCatalog);
                break;
            case 'S':
            case 's':
                sellProduct(productCatalog);
                break;
            case 'L':
            case 'l':
                listProducts(productCatalog);
                break;
            case 'R':
            case 'r':
                removeProduct(productCatalog);
                break;
            case 'T':
            case 't':
                calculateTotalBasePrice(productCatalog);
                break;
            case 'D':
            case 'd':
                changeDollarRate();
                break;
            case 'Q':
            case 'q':
                std::cout << "Quitting the program." << std::endl;
                break;
            default:
                std::cerr << "Invalid choice. Please enter A, S, L, R, T, D, or Q." << std::endl;
                break;
        }

        // Clear the input buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    } while (choice != 'Q' && choice != 'q');

    saveCatalogToFile(productCatalog);  // Save the catalog before quitting
    saveDollarRate();  // Save the dollar rate before quitting
    return 0;
}
