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

    /**
     * @brief Constructs a @c Category from its JSON representation.
     *
     * Validates the type of each expected field before building the object.
     *
     * @param json The JSON object to deserialize.
     *
     * @return A newly created @c Category.
     *
     * @throws std::invalid_argument If any field is missing or has the wrong type.
     */
    static std::unique_ptr<Category> fromJSON(const json& json);

    /**
     * @brief Serializes this category into a JSON object.
     *
     * Writes the category ID, name and monthly budget.
     *
     * @return A JSON object representing this category.
     */
    json toJSON() const;

    // Equality compares all fields
    bool operator==(const Category& other) const;
    // Friend so it can write the private fields directly to any stream
    friend ostream& operator<<(ostream& os, const Category& c);
};
#endif //EXPENSETRACKER_CATEGORY_H
