#ifndef EXPENSETRACKER_INCOME_H
#define EXPENSETRACKER_INCOME_H

#include <string>

#include "Transaction.h"
#include "JSON.h"

using namespace std;

class Income : public Transaction {
protected:
    string source;
    // Helper function for checking source
    static bool isValidSource(const string& src);

public:
    // Constructor validating the source before delegating to Transaction
    Income(const string& id, const string& date, double amount, const string& category, const string& description, const string& source);
    // Destructor
    ~Income() override;

    // Getter for the source
    string getSource() const;
    // Setter for the source, validated before assignment
    void setSource(const string& newSource);

    // Income never reduces the budget
    bool computeImpact() const override;
    // Prints the income's fields to standard output
    void displayTransaction() const override;

    /**
     * @brief Constructs an @c Income from its JSON representation.
     *
     * Validates the type of each expected field before building the object.
     *
     * @param json The JSON object to deserialize.
     *
     * @return A newly created @c Income.
     *
     * @throws std::invalid_argument If any field is missing or has the wrong type.
     */
    static std::unique_ptr<Income> fromJSON(const json& json);

    /**
     * @brief Serializes this income into a JSON object.
     *
     * Extends @c Transaction::toJSON with the @c __type discriminator (@c 2) and the source.
     *
     * @return A JSON object representing this income.
     */
    json toJSON() const override;
};
#endif //EXPENSETRACKER_INCOME_H