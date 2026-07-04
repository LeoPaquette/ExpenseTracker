#include "../include/Expense.h"

#include <string>
#include <iostream>
using namespace std;

bool Expense::isValidPaymentMethod(const string& method) {
    string trimmed = trim(method);
    return !trimmed.empty() && trimmed.length() <= 30;
}

Expense::Expense(const string &id, const string &date, const string &amount, const string &category, const string &description, const string &paymentMethod)
    :Transaction(id, date, amount, category, description) {
    if (!isValidPaymentMethod(paymentMethod)) {
        throw invalid_argument("Invalid payment method");
    }
    this->paymentMethod = paymentMethod;
}

Expense::~Expense() {}

string Expense::getPaymentMethod() const {
    return paymentMethod;
}

void Expense::setPaymentMethod(const string &newPaymentMethod) {
    if (!isValidPaymentMethod(newPaymentMethod)) {
        throw invalid_argument("Invalid payment method");
    }
    paymentMethod = newPaymentMethod;
}

bool Expense::computeImpact() const{
    return true;
}

void Expense::displayTransaction() const{
    cout << "ID: " << transactionID
         << ", Date: " << date
         << ", Amount: " << amount
         << ", Category: " << category
         << ", Description: " << description
         << ", Payment Method: " << paymentMethod << endl;
}