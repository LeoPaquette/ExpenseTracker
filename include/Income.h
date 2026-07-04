#ifndef EXPENSETRACKER_INCOME_H
#define EXPENSETRACKER_INCOME_H

#include "Transaction.h"
#include <string>
using namespace std;

class Income : public Transaction {
protected:
    string source;
    static bool isValidSource(const string& src);

public:
    Income(const string& id, const string& date, const string& amount, const string& category, const string& description, const string& source);
    ~Income() override;

    string getSource() const;
    void setSource(const string& newSource);

    bool computeImpact() const override;
    void displayTransaction() const override;
};
#endif //EXPENSETRACKER_INCOME_H