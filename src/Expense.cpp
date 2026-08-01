#include "../include/Expense.h"

#include <string>
using namespace std;

// Helper function for checking payment method
bool Expense::isValidPaymentMethod(const string& method) {
    string trimmed = trim(method);
    return !trimmed.empty() && trimmed.length() <= 30;
}

// Constructor validating the payment method before delegating to Transaction
Expense::Expense(const string &id, const string &date, double amount, const string &category, const string &description, const string &paymentMethod)
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

// Returns a human-readable summary of the expense's fields, built from Transaction::displayTransaction
string Expense::displayTransaction() const {
    return Transaction::displayTransaction() + ", Payment Method: " + paymentMethod;
}


std::unique_ptr<Expense> Expense::fromJSON(const json& json) {
    if (const auto type = json.at("__type"); !type.is_number()) {
        throw invalid_argument("Invalid type. Must be a number.");
    }

    const auto transactionId = json.at("transactionId");
    if (!transactionId.is_string()) {
        throw invalid_argument("Invalid transaction ID. Must be a string.");
    }

    const auto date = json.at("date");
    if (!date.is_string()) {
        throw invalid_argument("Invalid date. Must be a string.");
    }

    const auto amount = json.at("amount");
    if (!amount.is_number_float()) {
        throw invalid_argument("Invalid amount. Must be a float/double.");
    }

    const auto category = json.at("category");
    if (!category.is_string()) {
        throw invalid_argument("Invalid category. Must be a string.");
    }

    const auto description = json.at("description");
    if (!description.is_string()) {
        throw invalid_argument("Invalid description. Must be a string.");
    }

    const auto paymentMethod = json.at("paymentMethod");
    if (!paymentMethod.is_string()) {
        throw invalid_argument("Invalid payment method. Must be a string.");
    }

    return std::unique_ptr<Expense>(new Expense(
        transactionId,
        date,
        amount.get<double>(),
        category,
        description,
        paymentMethod
    ));
}

json Expense::toJSON() const {
    json json = Transaction::toJSON();

    json["__type"] = 1;
    json["paymentMethod"] = this->paymentMethod;

    return json;
}