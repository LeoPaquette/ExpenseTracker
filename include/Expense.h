#ifndef EXPENSETRACKER_EXPENSE_H
#define EXPENSETRACKER_EXPENSE_H

#include <string>

#include "Transaction.h"
#include "JSON.h"

using namespace std;

class Expense : public Transaction {
protected:
    string paymentMethod;
    // Helper function for checking payment method
    static bool isValidPaymentMethod(const string& method);
public:
    // Constructor validating the payment method before delegating to Transaction
    Expense(const string& id, const string& date, double amount, const string& category, const string& description, const string& paymentMethod);
    // Destructor
    ~Expense() override;

    // Getter for the payment method
    string getPaymentMethod() const;
    // Setter for the payment method, validated before assignment
    void setPaymentMethod(const string& newPaymentMethod);

    // Expenses always reduce the budget
    bool computeImpact() const override;
    // Returns a human-readable summary of the expense's fields, built from Transaction::displayTransaction
    string displayTransaction() const override;

    /**
     * @brief Constructs an @c Expense from its JSON representation.
     *
     * Validates the type of each expected field before building the object.
     *
     * @param json The JSON object to deserialize.
     *
     * @return A newly created @c Expense.
     *
     * @throws std::invalid_argument If any field is missing or has the wrong type.
     */
    static std::unique_ptr<Expense> fromJSON(const json& json);

    /**
     * @brief Serializes this expense into a JSON object.
     *
     * Extends @c Transaction::toJSON with the @c __type discriminator (@c 1) and the
     * payment method.
     *
     * @return A JSON object representing this expense.
     */
    json toJSON() const override;
};
#endif //EXPENSETRACKER_EXPENSE_H