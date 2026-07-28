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
    // Prints the expense's fields to standard output
    void displayTransaction() const override;

    static std::unique_ptr<Expense> fromJSON(const json& json);

    json toJSON() const override;
};
#endif //EXPENSETRACKER_EXPENSE_H