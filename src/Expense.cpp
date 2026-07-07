#include "../include/Expense.h"

#include <string>
#include <iostream>
using namespace std;

// Helper function for checking payment method
bool Expense::isValidPaymentMethod(const string& method) {
    string trimmed = trim(method);
    return !trimmed.empty() && trimmed.length() <= 30;
}

// Constructor validating the payment method before delegating to Transaction
Expense::Expense(const string &id, const string &date, const string &amount, const string &category, const string &description, const string &paymentMethod)
    :Transaction(id, date, amount, category, description) {
    if (!isValidPaymentMethod(paymentMethod)) {
        throw invalid_argument("Invalid payment method");
    }
    this->paymentMethod = paymentMethod;
}

// Destructor
Expense::~Expense() {}

// Getter for the payment method
string Expense::getPaymentMethod() const {
    return paymentMethod;
}

// Setter for the payment method, validated before assignment
void Expense::setPaymentMethod(const string &newPaymentMethod) {
    if (!isValidPaymentMethod(newPaymentMethod)) {
        throw invalid_argument("Invalid payment method");
    }
    paymentMethod = newPaymentMethod;
}

// Expenses always reduce the budget
bool Expense::computeImpact() const{
    return true;
}

// Prints the expense's fields to standard output
void Expense::displayTransaction() const{
    cout << "ID: " << transactionID
         << ", Date: " << date
         << ", Amount: " << amount
         << ", Category: " << category
         << ", Description: " << description
         << ", Payment Method: " << paymentMethod << endl;
}