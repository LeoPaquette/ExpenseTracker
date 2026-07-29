#ifndef EXPENSETRACKER_TRANSACTION_H
#define EXPENSETRACKER_TRANSACTION_H

#include <string>
#include <iostream>

#include "JSON.h"

using namespace std;

class Transaction {
protected:
    string transactionID;
    string date;
    double amount = 0.0;
    string category;
    string description;

    // Helper function for checking ID
    static bool isValidID(const string& id);
    // Helper function for checking amount limits
    static bool isValidAmount(double amount);
    // Helper function for checking category
    static bool isValidCategory(const string& category);
    // Helper function for checking description
    static bool isValidDescription(const string& description);
    // Helper function for trimming strings
    static string trim(const string& s);

public:
    // Constructor validating and trimming all fields before assignment
    Transaction(const string& id, const string& date, double amount, const string& category, const string& description);
    // Destructor
    virtual ~Transaction();

    // Helper function for checking dates
    static bool isValidDate(const string& date);

    // Getter for the transaction ID
    string getTransactionID() const;
    // Getter for the date
    string getDate() const;
    // Getter for the amount
    double getAmount() const;
    // Getter for the category
    string getCategory() const;
    // Getter for the description
    string getDescription() const;

    // Setter for the date, validated before assignment
    void setDate(const string& newDate);
    // Setter for the amount, validated before assignment
    void setAmount(double newAmount);
    // Setter for the category, validated before assignment
    void setCategory(const string& newCategory);
    // Setter for the description, validated before assignment
    void setDescription(const string& newDescription);

    // Computes whether this transaction reduces the budget; overridden per transaction type
    virtual bool computeImpact() const = 0;
    // Prints the transaction's fields to standard output
    virtual void displayTransaction() const;

    // Equality compares all shared fields
    bool operator==(const Transaction& other) const;
    // Friend so it can write the protected fields directly to any stream
    friend ostream& operator<<(ostream& os, const Transaction& t);

    /**
     * TODO: Document
     */
    static std::unique_ptr<Transaction> fromJSON(const json& json);

    /**
     * TODO: Document
     */
    virtual json toJSON() const;
};
#endif //EXPENSETRACKER_TRANSACTION_H