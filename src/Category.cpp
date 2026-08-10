#include "../include/Category.h"

#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <iomanip>

#include "include/Util.h"

using namespace std;

// Helper function for checking category ID
bool Category::isValidID(const string& id) {
    static const regex pattern(R"(^CAT-\d{4}$)");
    return !id.empty() && regex_match(id, pattern);
}

// Helper function for checking name
bool Category::isValidName(const string& name) {
    string trimmed = trim(name);
    return !trimmed.empty() && trimmed.length() <= 30;
}

// Helper function for checking monthly budget
bool Category::isValidBudget(double budget) {
    return budget > 0.0 && budget <= 1000000.0;
}

// Helper function for trimming strings
string Category::trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == string::npos) return ""; // string is all whitespace
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

// Constructor validating and trimming all fields before assignment
Category::Category(const string& categoryID, const string& name, double monthlyBudget) {
    string trimmedID = trim(categoryID);
    string trimmedName = trim(name);

    // Validate each field individually so the error message identifies which one failed
    if (!isValidID(trimmedID)) {
        throw invalid_argument("Invalid category ID. Expected format CAT-0000.");
    }
    if (!isValidName(trimmedName)) {
        throw invalid_argument("Invalid category name. Must be 1-30 characters.");
    }
    if (!isValidBudget(monthlyBudget)) {
        throw invalid_argument("Invalid monthly budget. Must be a number between 0 and 1,000,000.");
    }

    this->categoryID = trimmedID;
    this->name = trimmedName;
    this->monthlyBudget = monthlyBudget;
}

// Destructor
Category::~Category() {
}

// Getter for the category ID
string Category::getCategoryID() const {
    return categoryID;
}

// Getter for the name
string Category::getName() const {
    return name;
}

// Getter for the monthly budget
double Category::getMonthlyBudget() const {
    return monthlyBudget;
}

// Setter for the name, validated and trimmed before assignment
void Category::setName(std::string name) {
    string trimmed = trim(name);
    if (!isValidName(trimmed)) {
        throw invalid_argument("Invalid category name. Must be 1-30 characters.");
    }
    this->name = trimmed;
}

// Setter for the monthly budget, validated before assignment
void Category::setMonthlyBudget(double monthlyBudget) {
    if (monthlyBudget <= 0.0 || monthlyBudget > 1000000.0) {
        throw invalid_argument("Invalid monthly budget. Must be between 0 and 1,000,000.");
    }
    this->monthlyBudget = monthlyBudget;
}

// Replaces the monthly budget with a newly validated value
void Category::updateBudget(double newMonthlyBudget) {
    setMonthlyBudget(newMonthlyBudget);
}

// Returns the category's fields plus a spending summary and any budget warning, built from operator<<
string Category::displayCategorySummary(double amountSpent) const {
    ostringstream oss;
    oss << *this
        << ", Spent: " << amountSpent
        << ", Remaining: " << (monthlyBudget - amountSpent);

    // Guard against dividing by a zero budget rather than crashing or printing garbage
    if (monthlyBudget <= 0.0) {
        oss << ", Usage: N/A (no budget set)"
            << "\nWarning: This category has no budget set.";
        return oss.str();
    }

    const double usagePercent = (amountSpent / monthlyBudget) * 100.0;
    oss << ", Usage: " << fixed << setprecision(1) << usagePercent << "%" << defaultfloat;

    if (usagePercent > 100.0) {
        oss << "\nWarning: Budget usage has exceeded 100% for category \"" << name << "\".";
    } else if (usagePercent > 80.0) {
        oss << "\nWarning: Budget usage has exceeded 80% for category \"" << name << "\".";
    }

    return oss.str();
}


std::unique_ptr<Category> Category::fromJSON(const json& json) {
    const auto categoryId = json.at("categoryId");
    if (!categoryId.is_string()) {
        throw invalid_argument("Invalid category ID. Must be a string.");
    }

    const auto name = json.at("name");
    if (!name.is_string()) {
        throw invalid_argument("Invalid name. Must be a string.");
    }

    const auto monthlyBudget = json.at("monthlyBudget");
    if (!monthlyBudget.is_number_float()) {
        throw invalid_argument("Invalid monthly budget. Must be a float/double.");
    }

    return std::make_unique<Category>(categoryId, name, monthlyBudget.get<double>());
}

json Category::toJSON() const {
    json json;

    json["categoryId"] = this->categoryID;
    json["name"] = this->name;
    json["monthlyBudget"] = this->monthlyBudget;

    return json;
}

// Equality compares all fields
bool Category::operator==(const Category& other) const {
    return categoryID == other.categoryID
        && util::insensitive_equals(name, other.name)
        && monthlyBudget == other.monthlyBudget;
}

// Friend function writing a category's fields to the given stream
ostream& operator<<(ostream& os, const Category& c) {
    os << "Category ID: " << c.categoryID
       << ", Name: " << c.name
       << ", Monthly Budget: " << c.monthlyBudget;
    return os;
}