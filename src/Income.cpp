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
Income::Income(const string& id, const string& date, double amount, const string& category, const string& description, const string& source)
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


std::unique_ptr<Income> Income::fromJSON(const json& json) {
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

    const auto source = json.at("source");
    if (!source.is_string()) {
        throw invalid_argument("Invalid source. Must be a string.");
    }

    return std::unique_ptr<Income>(new Income(
        transactionId,
        date,
        amount.get<double>(),
        category,
        description,
        source
    ));
}

json Income::toJSON() const {
    json json = Transaction::toJSON();

    json["__type"] = 2;
    json["source"] = this->source;

    return json;
}