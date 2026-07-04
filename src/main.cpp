#include "../include/Transaction.h"
#include "../include/Expense.h"

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

class ExpenseValidatorTester : public Expense {
public:
    using Expense::isValidPaymentMethod;
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

void testPaymentMethod() {
    string method = prompt("Payment Method: ");
    cout << (ExpenseValidatorTester::isValidPaymentMethod(method) ? "VALID" : "INVALID") << "\n";
}

void testCreateExpense() {
    string id = prompt("Transaction ID: ");
    string date = prompt("Date: ");
    string amount = prompt("Amount: ");
    string category = prompt("Category: ");
    string description = prompt("Description: ");
    string paymentMethod = prompt("Payment Method: ");

    try {
        Expense expense(id, date, amount, category, description, paymentMethod);
        cout << "Expense created successfully.\n";
        expense.displayTransaction();
        cout << "computeImpact(): " << (expense.computeImpact() ? "true" : "false") << "\n";
    } catch (const invalid_argument& e) {
        cout << "Failed to create Expense: " << e.what() << "\n";
    }
}

void testSetPaymentMethod() {
    string id = prompt("Transaction ID (for a valid base Expense): ");
    string date = prompt("Date: ");
    string amount = prompt("Amount: ");
    string category = prompt("Category: ");
    string description = prompt("Description: ");
    string paymentMethod = prompt("Payment Method: ");

    try {
        Expense expense(id, date, amount, category, description, paymentMethod);
        string newPaymentMethod = prompt("New Payment Method: ");
        try {
            expense.setPaymentMethod(newPaymentMethod);
            cout << "Payment method updated to: " << expense.getPaymentMethod() << "\n";
        } catch (const invalid_argument& e) {
            cout << "setPaymentMethod rejected: " << e.what() << "\n";
        }
    } catch (const invalid_argument& e) {
        cout << "Failed to create base Expense: " << e.what() << "\n";
    }
}

int main() {
    cout << "=== Transaction / Expense Tester ===\n";

    while (true) {
        cout << "\nChoose a function to test:\n"
             << "  1) isValidID\n"
             << "  2) isValidDate\n"
             << "  3) isValidAmount\n"
             << "  4) isValidCategory\n"
             << "  5) isValidDescription\n"
             << "  6) trim\n"
             << "  7) isValidPaymentMethod (Expense)\n"
             << "  8) Create an Expense (displayTransaction / computeImpact)\n"
             << "  9) setPaymentMethod on an Expense\n"
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
        else if (choice == "7") testPaymentMethod();
        else if (choice == "8") testCreateExpense();
        else if (choice == "9") testSetPaymentMethod();
        else cout << "Unrecognized option.\n";
    }

    cout << "Goodbye.\n";
    return 0;
}
