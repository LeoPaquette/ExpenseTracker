#ifndef EXPENSETRACKER_TRANSACTION_H
#define EXPENSETRACKER_TRANSACTION_H

#include <string>
#include <iostream>
#include <set>
using namespace std;

class Transaction {
protected:
    string transactionID;
    string date;
    double amount;
    string category;
    string description;
    static set<string> usedCategories;

    static bool isValidID(string& id);
    static bool isValidDate(string& date);
    static bool isValidAmount(string& amount);
    static bool isValidCategory(string& category);
    static bool isValidDescription(string& description);
    static string trim(string& s);

public:
    Transaction(string& id, string& date, string& amount, string& category, string& description);
    virtual ~Transaction();
    string getTransactionID();
    string getDate();
    double getAmount();
    string getCategory();
    string getDescription();

    void setDate(string& newDate);
    void setAmount(double newAmount);
    void setCategory(string& newCategory);
    void setDescription(string& newDescription);

    virtual bool computeImpact() = 0;
    virtual void printTransaction();
};
#endif //EXPENSETRACKER_TRANSACTION_H
