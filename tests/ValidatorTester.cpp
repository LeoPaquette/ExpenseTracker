#include "../include/Transaction.h"
#include "../include/Expense.h"
#include "../include/Income.h"
#include "../include/Category.h"

#include <iostream>
#include <string>

using namespace std;

// Test helper exposing Transaction's protected validators
class TransactionValidatorTester : public Transaction {
public:
    using Transaction::isValidID;
    using Transaction::isValidDate;
    using Transaction::isValidAmount;
    using Transaction::isValidCategory;
    using Transaction::isValidDescription;
    using Transaction::trim;
};

// Test helper exposing Expense's protected validator
class ExpenseValidatorTester : public Expense {
public:
    using Expense::isValidPaymentMethod;
};

// Test helper exposing Income's protected validator
class IncomeValidatorTester : public Income {
public:
    using Income::isValidSource;
};

// Test helper exposing Category's protected validators
class CategoryValidatorTester : public Category {
public:
    using Category::isValidID;
    using Category::isValidName;
    using Category::isValidBudget;
    using Category::trim;
};

// Helper function for reading a labeled line of input from the console
string prompt(const string& label) {
    string value;
    cout << label;
    getline(cin, value);
    return value;
}

// Prompts for a transaction ID and reports whether it passes validation
void testID() {
    string id = prompt("Transaction ID: ");
    cout << (TransactionValidatorTester::isValidID(id) ? "VALID" : "INVALID") << "\n";
}

// Prompts for a date and reports whether it passes validation
void testDate() {
    string date = prompt("Date: ");
    cout << (TransactionValidatorTester::isValidDate(date) ? "VALID" : "INVALID") << "\n";
}

// Prompts for an amount and reports whether it passes validation
void testAmount() {
    string amount = prompt("Amount: ");
    cout << (TransactionValidatorTester::isValidAmount(amount) ? "VALID" : "INVALID") << "\n";
}

// Prompts for a category and reports whether it passes validation
void testCategory() {
    string category = prompt("Category (must be unique, case-insensitive): ");
    cout << (TransactionValidatorTester::isValidCategory(category) ? "VALID" : "INVALID") << "\n";
}

// Prompts for a description and reports whether it passes validation
void testDescription() {
    string description = prompt("Description: ");
    cout << (TransactionValidatorTester::isValidDescription(description) ? "VALID" : "INVALID") << "\n";
}

// Prompts for a string and prints its trimmed result
void testTrim() {
    string s = prompt("String to trim: ");
    cout << "Trimmed: \"" << TransactionValidatorTester::trim(s) << "\"\n";
}

// Prompts for a payment method and reports whether it passes validation
void testPaymentMethod() {
    string method = prompt("Payment Method: ");
    cout << (ExpenseValidatorTester::isValidPaymentMethod(method) ? "VALID" : "INVALID") << "\n";
}

// Prompts for all Expense fields, constructs one, and displays the result
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

// Builds a base Expense, then prompts for and applies a new payment method
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

// Prompts for an income source and reports whether it passes validation
void testSource() {
    string source = prompt("Income Source: ");
    cout << (IncomeValidatorTester::isValidSource(source) ? "VALID" : "INVALID") << "\n";
}

// Prompts for all Income fields, constructs one, and displays the result
void testCreateIncome() {
    string id = prompt("Transaction ID: ");
    string date = prompt("Date: ");
    string amount = prompt("Amount: ");
    string category = prompt("Category: ");
    string description = prompt("Description: ");
    string source = prompt("Source: ");

    try {
        Income income(id, date, amount, category, description, source);
        cout << "Income created successfully.\n";
        income.displayTransaction();
        cout << "computeImpact(): " << (income.computeImpact() ? "true" : "false") << "\n";
    } catch (const invalid_argument& e) {
        cout << "Failed to create Income: " << e.what() << "\n";
    }
}

// Builds a base Income, then prompts for and applies a new source
void testSetSource() {
    string id = prompt("Transaction ID (for a valid base Income): ");
    string date = prompt("Date: ");
    string amount = prompt("Amount: ");
    string category = prompt("Category: ");
    string description = prompt("Description: ");
    string source = prompt("Source: ");

    try {
        Income income(id, date, amount, category, description, source);
        string newSource = prompt("New Source: ");
        try {
            income.setSource(newSource);
            cout << "Source updated to: " << income.getSource() << "\n";
        } catch (const invalid_argument& e) {
            cout << "setSource rejected: " << e.what() << "\n";
        }
    } catch (const invalid_argument& e) {
        cout << "Failed to create base Income: " << e.what() << "\n";
    }
}

// Prompts for a category ID and reports whether it passes validation
void testCategoryID() {
    string id = prompt("Category ID: ");
    cout << (CategoryValidatorTester::isValidID(id) ? "VALID" : "INVALID") << "\n";
}

// Prompts for a category name and reports whether it passes validation
void testCategoryName() {
    string name = prompt("Category Name (must be unique, case-insensitive): ");
    cout << (CategoryValidatorTester::isValidName(name) ? "VALID" : "INVALID") << "\n";
}

// Prompts for a monthly budget and reports whether it passes validation
void testCategoryBudget() {
    string budget = prompt("Monthly Budget: ");
    cout << (CategoryValidatorTester::isValidBudget(budget) ? "VALID" : "INVALID") << "\n";
}

// Prompts for all Category fields, constructs one, and displays the result
void testCreateCategory() {
    string id = prompt("Category ID: ");
    string name = prompt("Category Name: ");
    string budget = prompt("Monthly Budget: ");

    try {
        Category category(id, name, budget);
        cout << "Category created successfully.\n";
        cout << category << "\n";
    } catch (const invalid_argument& e) {
        cout << "Failed to create Category: " << e.what() << "\n";
    }
}

// Builds a base Category, then prompts for and applies a new monthly budget
void testUpdateBudget() {
    string id = prompt("Category ID (for a valid base Category): ");
    string name = prompt("Category Name: ");
    string budget = prompt("Monthly Budget: ");

    try {
        Category category(id, name, budget);
        string newBudget = prompt("New Monthly Budget: ");
        try {
            category.updateBudget(stod(newBudget));
            cout << "Monthly budget updated to: " << category.getMonthlyBudget() << "\n";
        } catch (const exception& e) {
            cout << "updateBudget rejected: " << e.what() << "\n";
        }
    } catch (const invalid_argument& e) {
        cout << "Failed to create base Category: " << e.what() << "\n";
    }
}

// Builds a Category, then prompts for an amount spent and displays its budget summary
void testDisplayCategorySummary() {
    string id = prompt("Category ID: ");
    string name = prompt("Category Name: ");
    string budget = prompt("Monthly Budget: ");

    try {
        Category category(id, name, budget);
        string amountSpent = prompt("Amount Spent So Far: ");
        try {
            category.displayCategorySummary(stod(amountSpent));
        } catch (const exception& e) {
            cout << "Invalid amount spent: " << e.what() << "\n";
        }
    } catch (const invalid_argument& e) {
        cout << "Failed to create Category: " << e.what() << "\n";
    }
}

// Prompts for two Categories, prints each via operator<<, then compares them with operator==
void testCompareCategories() {
    cout << "-- First category --\n";
    string id1 = prompt("Category ID: ");
    string name1 = prompt("Category Name: ");
    string budget1 = prompt("Monthly Budget: ");

    cout << "-- Second category --\n";
    string id2 = prompt("Category ID: ");
    string name2 = prompt("Category Name: ");
    string budget2 = prompt("Monthly Budget: ");

    try {
        Category first(id1, name1, budget1);
        Category second(id2, name2, budget2);

        cout << "First:  " << first << "\n";
        cout << "Second: " << second << "\n";
        cout << (first == second ? "EQUAL" : "NOT EQUAL") << "\n";
    } catch (const invalid_argument& e) {
        cout << "Failed to create category: " << e.what() << "\n";
    }
}

// Prompts for two Expenses, prints each via operator<<, then compares them with operator==
void testCompareTransactions() {
    cout << "-- First transaction --\n";
    string id1 = prompt("Transaction ID: ");
    string date1 = prompt("Date: ");
    string amount1 = prompt("Amount: ");
    string category1 = prompt("Category: ");
    string description1 = prompt("Description: ");
    string paymentMethod1 = prompt("Payment Method: ");

    cout << "-- Second transaction --\n";
    string id2 = prompt("Transaction ID: ");
    string date2 = prompt("Date: ");
    string amount2 = prompt("Amount: ");
    string category2 = prompt("Category: ");
    string description2 = prompt("Description: ");
    string paymentMethod2 = prompt("Payment Method: ");

    try {
        Expense first(id1, date1, amount1, category1, description1, paymentMethod1);
        Expense second(id2, date2, amount2, category2, description2, paymentMethod2);

        cout << "First:  " << first << "\n";
        cout << "Second: " << second << "\n";
        cout << (first == second ? "EQUAL" : "NOT EQUAL") << "\n";
    } catch (const invalid_argument& e) {
        cout << "Failed to create transaction: " << e.what() << "\n";
    }
}

int main() {
    cout << "=== Transaction / Expense / Income Tester ===\n";

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
             << " 10) isValidSource (Income)\n"
             << " 11) Create an Income (displayTransaction / computeImpact)\n"
             << " 12) setSource on an Income\n"
             << " 13) Compare two transactions (operator== / operator<<)\n"
             << " 14) isValidID (Category)\n"
             << " 15) isValidName (Category)\n"
             << " 16) isValidBudget (Category)\n"
             << " 17) Create a Category\n"
             << " 18) updateBudget on a Category\n"
             << " 19) displayCategorySummary on a Category\n"
             << " 20) Compare two categories (operator== / operator<<)\n"
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
        else if (choice == "10") testSource();
        else if (choice == "11") testCreateIncome();
        else if (choice == "12") testSetSource();
        else if (choice == "13") testCompareTransactions();
        else if (choice == "14") testCategoryID();
        else if (choice == "15") testCategoryName();
        else if (choice == "16") testCategoryBudget();
        else if (choice == "17") testCreateCategory();
        else if (choice == "18") testUpdateBudget();
        else if (choice == "19") testDisplayCategorySummary();
        else if (choice == "20") testCompareCategories();
        else cout << "Unrecognized option.\n";
    }
    return 0;
}
