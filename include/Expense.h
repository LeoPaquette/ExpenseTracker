#ifndef EXPENSETRACKER_EXPENSE_H
#define EXPENSETRACKER_EXPENSE_H

#include "Transaction.h"
#include <string>
using namespace std;

class Expense : public Transaction {
protected:
    string paymentMethod;
    static bool isValidPaymentMethod(const string& method);
public:
    Expense(const string& id, const string& date, const string& amount, const string& category, const string& description, const string& paymentMethod);
    ~Expense() override;

    string getPaymentMethod() const;
    void setPaymentMethod(const string& newPaymentMethod);

    bool computeImpact() const override;
    void displayTransaction() const override;
};
#endif //EXPENSETRACKER_EXPENSE_H