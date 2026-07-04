#ifndef EXPENSETRACKER_TRANSACTION_H
#define EXPENSETRACKER_TRANSACTION_H

#include <string>
#include <iostream>
using namespace std;

class Transaction {
protected:
    string transactionID;
    string date;
    double amount = 0.0;
    string category;
    string description;

    static bool isValidID(const string& id);
    static bool isValidDate(const string& date);
    static bool isValidAmount(const string& amount);
    static bool isValidCategory(const string& category);
    static bool isValidDescription(const string& description);
    static string trim(const string& s);

public:
    Transaction(const string& id, const string& date, const string& amount, const string& category, const string& description);
    virtual ~Transaction();

    string getTransactionID() const;
    string getDate() const;
    double getAmount() const;
    string getCategory() const;
    string getDescription() const;

    void setDate(const string& newDate);
    void setAmount(double newAmount);
    void setCategory(const string& newCategory);
    void setDescription(const string& newDescription);

    virtual bool computeImpact() const = 0;
    virtual void printTransaction() const;
};
#endif //EXPENSETRACKER_TRANSACTION_H