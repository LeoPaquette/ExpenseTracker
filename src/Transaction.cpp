#include "../include/Transaction.h"
#include <regex>
#include <string>
#include <algorithm>
#include <cctype>

using namespace std;


// Helper function for checking ID
bool Transaction::isValidID(const string& id) {
    static const regex pattern(R"(^TRX-\d{4}$)");
    return !id.empty() && regex_match(id, pattern);
}

// Helper function for checking dates
bool Transaction::isValidDate(const string& date) {
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
bool Transaction::isValidAmount(const string& amount) {
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
bool Transaction::isValidCategory(const string& category) {
    string trimmed = trim(category);
    return !trimmed.empty() && trimmed.length() <= 30;
}

// Helper function for checking description
bool Transaction::isValidDescription(const string& description) {
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
string Transaction::trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

Transaction::Transaction(const string& id, const string& date, const string& amount,
                         const string& category, const string& description) {
    string trimmedID = trim(id);
    string trimmedDate = trim(date);
    string trimmedAmount = trim(amount);
    string trimmedCategory = trim(category);
    string trimmedDescription = trim(description);

    if (!isValidID(trimmedID))
        throw invalid_argument("Invalid transaction ID. Expected format TRX-0000.");
    if (!isValidDate(trimmedDate))
        throw invalid_argument("Invalid date. Expected a real, non-future date in YYYY-MM-DD.");
    if (!isValidAmount(trimmedAmount))
        throw invalid_argument("Invalid amount. Must be a number between 0 and 1,000,000.");
    if (!isValidCategory(trimmedCategory))
        throw invalid_argument("Invalid category. Must be 1-30 characters.");
    if (!isValidDescription(trimmedDescription))
        throw invalid_argument("Invalid description. Must be 5-100 characters and contain a letter.");

    transactionID = trimmedID;
    this->date = trimmedDate;
    this->amount = stod(trimmedAmount);
    this->category = trimmedCategory;
    this->description = trimmedDescription;
}

Transaction::~Transaction() {
}

string Transaction::getTransactionID() const {
    return transactionID;
}

string Transaction::getDate() const {
    return date;
}

double Transaction::getAmount() const {
    return amount;
}

string Transaction::getCategory() const {
    return category;
}

string Transaction::getDescription() const {
    return description;
}

void Transaction::setDate(const string& newDate) {
    string trimmed = trim(newDate);
    if (!isValidDate(trimmed)) {
        throw invalid_argument("Invalid date. Expected a real, non-future date in YYYY-MM-DD.");
    }
    date = trimmed;
}

void Transaction::setAmount(double newAmount) {
    if (newAmount <= 0.0 || newAmount > 1000000.0) {
        throw invalid_argument("Invalid amount. Must be between 0 and 1,000,000.");
    }
    amount = newAmount;
}

void Transaction::setCategory(const string& newCategory) {
    string trimmed = trim(newCategory);
    if (!isValidCategory(trimmed)) {
        throw invalid_argument("Invalid category. Must be 1-30 characters.");
    }
    category = trimmed;
}

void Transaction::setDescription(const string& newDescription) {
    string trimmed = trim(newDescription);
    if (!isValidDescription(trimmed)) {
        throw invalid_argument("Invalid description. Must be 5-100 characters and contain a letter.");
    }
    description = trimmed;
}

void Transaction::displayTransaction() const {
    cout << "ID: " << transactionID
         << ", Date: " << date
         << ", Amount: " << amount
         << ", Category: " << category
         << ", Description: " << description << endl;
}