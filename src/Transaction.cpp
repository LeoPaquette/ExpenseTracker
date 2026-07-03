#include "../include/Transaction.h"
#include <regex>
#include <string>
#include <algorithm>
#include <cctype>

using namespace std;

set<string> Transaction::usedCategories;

// Helper function for checking ID
bool Transaction::isValidID(string& id) {
    static const regex pattern(R"(^TRX-\d{4}$)");
    return !id.empty() && regex_match(id, pattern);
}

// Helper function for checking dates
bool Transaction::isValidDate(string& date) {
    static const regex pattern(R"(^(\d{4})-(\d{2})-(\d{2})$)");
    smatch match;
    if (date.empty() || !regex_match(date, match, pattern)) {
        return false;
    }
    int year = stoi(match[1].str());
    int month = stoi(match[2].str());
    int day = stoi(match[3].str());
    if (month < 1 || month > 12) {
        return false;
    }

    static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    int maxDays = daysInMonth[month - 1];
    if (isLeapYear && month == 2) {
        maxDays = 29;
    }
    if (day < 1 || day > maxDays) {
        return false;
    }

    time_t currentTime = time(nullptr);
    tm* now = localtime(&currentTime);
    int currentYear = now->tm_year + 1900;
    int currentMonth = now->tm_mon + 1;
    int currentDay = now->tm_mday;

    if (year > currentYear || (year == currentYear && month > currentMonth) || (year == currentYear && month == currentMonth && day > currentDay)) {
        return false;
    }
    return true;
}

// Helper function for checking amount limits
bool Transaction::isValidAmount(string& amount) {
    string trimmed = trim(amount);
    if (trimmed.empty()) {
        return false;
    }

    size_t i = 0;
    bool hasDigit = false;
    bool hasDecimalPoint = false;
    for (; i < trimmed.length(); i++) {
        char c = trimmed[i];
        if (isdigit(c)) {
            hasDigit = true;
        } else if (c == '.' && !hasDecimalPoint) {
            hasDecimalPoint = true;
        } else {
            return false;
        }
    }
    if (!hasDigit) {
        return false;
    }

    double value = stod(trimmed);
    return value > 0.0 && value <= 1000000.0;
}

// Helper function for checking category
bool Transaction::isValidCategory(string& category) {
    string trimmedCategory = trim(category);
    if (trimmedCategory.empty() || trimmedCategory.length() > 30) {
        return false;
    }

    string normalizedCategory = trimmedCategory;
    transform(normalizedCategory.begin(), normalizedCategory.end(), normalizedCategory.begin(),
              [](unsigned char c) { return tolower(c); });

    if (usedCategories.count(normalizedCategory)) {
        return false;
    }
    usedCategories.insert(normalizedCategory);
    return true;
}

// Helper function for checking description
bool Transaction::isValidDescription(string& description) {
    string trimmedDescription = trim(description);
    if (trimmedDescription.length() < 5 || trimmedDescription.length() > 100) {
        return false;
    }
    bool hasAlphabetic = false;
    for (char c : trimmedDescription) {
        if (isalpha(c)) {
            hasAlphabetic = true;
            break;
        }
    }
    return hasAlphabetic;
}

// Helper function for trimming strings
string Transaction::trim(string& s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

Transaction::Transaction(string& id, string& date, string& amount, string& category, string& description) {
}

Transaction::~Transaction() {
}

string Transaction::getTransactionID() {
    return transactionID;
}

string Transaction::getDate() {
    return date;
}

double Transaction::getAmount() {
    return amount;
}

string Transaction::getCategory() {
    return category;
}

string Transaction::getDescription() {
    return description;
}

void Transaction::setDate(string& newDate) {
    date = newDate;
}

void Transaction::setAmount(double newAmount) {
    amount = newAmount;
}

void Transaction::setCategory(string& newCategory) {
    category = newCategory;
}

void Transaction::printTransaction() {
    cout << "ID: " << transactionID
         << ", Date: " << date
         << ", Amount: " << amount
         << ", Category: " << category
         << ", Description: " << description << endl;
}