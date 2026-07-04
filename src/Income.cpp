#include "../include/Income.h"
#include <stdexcept>
#include <string>

using namespace std;

bool Income::isValidSource(const string& src) {
    string trimmed = trim(src);
    return !trimmed.empty() && trimmed.length() <= 30;
}

Income::Income(const string& id, const string& date, const string& amount, const string& category, const string& description, const string& source)
    : Transaction(id, date, amount, category, description) {
    if (!isValidSource(source)) {
        throw invalid_argument("Invalid income source. Must be 1-30 characters.");
    }
    this->source = trim(source);
}

Income::~Income() {
}

string Income::getSource() const {
    return source;
}

void Income::setSource(const string& newSource) {
    if (!isValidSource(newSource)) {
        throw invalid_argument("Invalid income source. Must be 1-30 characters.");
    }
    source = trim(newSource);
}

bool Income::computeImpact() const {
    return false;
}

void Income::displayTransaction() const {
    cout << "[Income]  "
         << "ID: " << transactionID
         << ", Date: " << date
         << ", Amount: +" << amount
         << ", Category: " << category
         << ", Source: " << source
         << ", Description: " << description << endl;
}