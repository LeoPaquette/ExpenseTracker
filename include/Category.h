#ifndef EXPENSETRACKER_CATEGORY_H
#define EXPENSETRACKER_CATEGORY_H

#include <string>
#include <iostream>

#include "JSON.h"

using namespace std;

class Category {
    string categoryID;
    string name;
    double monthlyBudget = 0.0;

protected:
    // Helper function for checking category ID
    static bool isValidID(const string& id);
    // Helper function for checking name
    static bool isValidName(const string& name);
    // Helper function for checking monthly budget
    static bool isValidBudget(double budget);
    // Helper function for trimming strings
    static string trim(const string& s);

public:
    // Constructor validating and trimming all fields before assignment
    Category(const string& categoryID, const string& name, double monthlyBudget);
    // Destructor
    ~Category();

    // Getter for the category ID
    string getCategoryID() const;
    // Getter for the name
    string getName() const;
    // Getter for the monthly budget
    double getMonthlyBudget() const;

    // Setter for the name, validated and trimmed before assignment
    void setName(std::string name);
    // Setter for the monthly budget, validated before assignment
    void setMonthlyBudget(double monthlyBudget);

    // Replaces the monthly budget with a newly validated value
    void updateBudget(double newMonthlyBudget);
    // Prints the category's fields plus a spending summary given the amount spent so far
    void displayCategorySummary(double amountSpent) const;

    static std::unique_ptr<Category> fromJSON(const json& json);

    json toJSON() const;
};
#endif //EXPENSETRACKER_CATEGORY_H
