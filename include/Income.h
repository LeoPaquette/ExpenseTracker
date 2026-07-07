#ifndef EXPENSETRACKER_INCOME_H
#define EXPENSETRACKER_INCOME_H

#include "Transaction.h"
#include <string>
using namespace std;

class Income : public Transaction {
protected:
    string source;
    // Helper function for checking source
    static bool isValidSource(const string& src);

public:
    // Constructor validating the source before delegating to Transaction
    Income(const string& id, const string& date, const string& amount, const string& category, const string& description, const string& source);
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
};
#endif //EXPENSETRACKER_INCOME_H