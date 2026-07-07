#include "../include/Income.h"
#include <stdexcept>
#include <string>

using namespace std;

// Helper function for checking source
bool Income::isValidSource(const string& src) {
    string trimmed = trim(src);
    return !trimmed.empty() && trimmed.length() <= 30;
}

// Constructor validating the source before delegating to Transaction
Income::Income(const string& id, const string& date, const string& amount, const string& category, const string& description, const string& source)
    : Transaction(id, date, amount, category, description) {
    if (!isValidSource(source)) {
        throw invalid_argument("Invalid income source. Must be 1-30 characters.");
    }
    this->source = trim(source);
}

// Destructor
Income::~Income() {
}

// Getter for the source
string Income::getSource() const {
    return source;
}

// Setter for the source, validated before assignment
void Income::setSource(const string& newSource) {
    if (!isValidSource(newSource)) {
        throw invalid_argument("Invalid income source. Must be 1-30 characters.");
    }
    source = trim(newSource);
}

// Income never reduces the budget
bool Income::computeImpact() const {
    return false;
}

// Prints the income's fields to standard output
void Income::displayTransaction() const {
    cout << "[Income]  "
         << "ID: " << transactionID
         << ", Date: " << date
         << ", Amount: +" << amount
         << ", Category: " << category
         << ", Source: " << source
         << ", Description: " << description << endl;
}