#include "../include/Transaction.h"

#include <iostream>
#include <string>

using namespace std;

class TransactionValidatorTester : public Transaction {
public:
    using Transaction::isValidID;
    using Transaction::isValidDate;
    using Transaction::isValidAmount;
    using Transaction::isValidCategory;
    using Transaction::isValidDescription;
    using Transaction::trim;
};



string prompt(const string& label) {
    string value;
    cout << label;
    getline(cin, value);
    return value;
}

void testID() {
    string id = prompt("Transaction ID: ");
    cout << (TransactionValidatorTester::isValidID(id) ? "VALID" : "INVALID") << "\n";
}

void testDate() {
    string date = prompt("Date: ");
    cout << (TransactionValidatorTester::isValidDate(date) ? "VALID" : "INVALID") << "\n";
}

void testAmount() {
    string amount = prompt("Amount: ");
    cout << (TransactionValidatorTester::isValidAmount(amount) ? "VALID" : "INVALID") << "\n";
}

void testCategory() {
    string category = prompt("Category (must be unique, case-insensitive): ");
    cout << (TransactionValidatorTester::isValidCategory(category) ? "VALID" : "INVALID") << "\n";
}

void testDescription() {
    string description = prompt("Description: ");
    cout << (TransactionValidatorTester::isValidDescription(description) ? "VALID" : "INVALID") << "\n";
}

void testTrim() {
    string s = prompt("String to trim: ");
    cout << "Trimmed: \"" << TransactionValidatorTester::trim(s) << "\"\n";
}


int main() {
    cout << "=== Transaction Validator Tester ===\n";
    cout << "Exercises the validation helpers currently implemented in Transaction.cpp.\n";

    while (true) {
        cout << "\nChoose a function to test:\n"
             << "  1) isValidID\n"
             << "  2) isValidDate\n"
             << "  3) isValidAmount\n"
             << "  4) isValidCategory\n"
             << "  5) isValidDescription\n"
             << "  6) trim\n"
             << "  0) Quit\n"
             << "> ";

        string choice;
        if (!getline(cin, choice) || choice == "0") {
            break;
        }

        if (choice == "1") testID();
        else if (choice == "2") testDate();
        else if (choice == "3") testAmount();
        else if (choice == "4") testCategory();
        else if (choice == "5") testDescription();
        else if (choice == "6") testTrim();
        else cout << "Unrecognized option.\n";
    }

    cout << "Goodbye.\n";
    return 0;
}
