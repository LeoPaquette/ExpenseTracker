#include "../include/Transaction.h"
#include <regex>
#include <string>
#include <cctype>

#include "../include/Expense.h"
#include "../include/Income.h"

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

    // Days per month, adjusted for leap years
    static const std::array daysInMonth = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    int maxDays = daysInMonth[month - 1];
    if (isLeapYear && month == 2) {
        maxDays = 29;
    }
    if (day < 1 || day > maxDays) {
        return false;
    }

    // Reject future dates by comparing against today's date
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
bool Transaction::isValidAmount(double amount) {
    return amount > 0.0 && amount <= 1000000.0;
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
    // Require at least one letter so purely numeric/symbolic text is rejected
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
    // Locate first and last non-whitespace characters
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return ""; // string is all whitespace
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

// Constructor validating and trimming all fields before assignment
Transaction::Transaction(const string& id, const string& date, double amount,
                         const string& category, const string& description) {
    string trimmedID = trim(id);
    string trimmedDate = trim(date);
    string trimmedCategory = trim(category);
    string trimmedDescription = trim(description);

    // Validate each field individually so the error message identifies which one failed
    if (!isValidID(trimmedID)) {
        throw invalid_argument("Invalid transaction ID. Expected format TRX-0000.");
    }
    if (!isValidDate(trimmedDate)) {
        throw invalid_argument("Invalid date. Expected a real, non-future date in YYYY-MM-DD.");
    }
    if (!isValidAmount(amount)) {
        throw invalid_argument("Invalid amount. Must be a number between 0 and 1,000,000.");
    }
    if (!isValidCategory(trimmedCategory)) {
        throw invalid_argument("Invalid category. Must be 1-30 characters.");
    }
    if (!isValidDescription(trimmedDescription)) {
        throw invalid_argument("Invalid description. Must be 5-100 characters and contain a letter.");
    }
    transactionID = trimmedID;
    this->date = trimmedDate;
    this->amount = amount;
    this->category = trimmedCategory;
    this->description = trimmedDescription;
}

// Destructor
Transaction::~Transaction() {
}

// Getter for the transaction ID
string Transaction::getTransactionID() const {
    return transactionID;
}

// Getter for the date
string Transaction::getDate() const {
    return date;
}

// Getter for the amount
double Transaction::getAmount() const {
    return amount;
}

// Getter for the category
string Transaction::getCategory() const {
    return category;
}

// Getter for the description
string Transaction::getDescription() const {
    return description;
}

// Setter for the date, validated before assignment
void Transaction::setDate(const string& newDate) {
    string trimmed = trim(newDate);
    if (!isValidDate(trimmed)) {
        throw invalid_argument("Invalid date. Expected a real, non-future date in YYYY-MM-DD.");
    }
    date = trimmed;
}

// Setter for the amount, validated before assignment
void Transaction::setAmount(double newAmount) {
    if (newAmount <= 0.0 || newAmount > 1000000.0) {
        throw invalid_argument("Invalid amount. Must be between 0 and 1,000,000.");
    }
    amount = newAmount;
}

// Setter for the category, validated before assignment
void Transaction::setCategory(const string& newCategory) {
    string trimmed = trim(newCategory);
    if (!isValidCategory(trimmed)) {
        throw invalid_argument("Invalid category. Must be 1-30 characters.");
    }
    category = trimmed;
}

// Setter for the description, validated before assignment
void Transaction::setDescription(const string& newDescription) {
    string trimmed = trim(newDescription);
    if (!isValidDescription(trimmed)) {
        throw invalid_argument("Invalid description. Must be 5-100 characters and contain a letter.");
    }
    description = trimmed;
}

// Prints the transaction's fields to standard output
void Transaction::displayTransaction() const {
    cout << "ID: " << transactionID
         << ", Date: " << date
         << ", Amount: " << amount
         << ", Category: " << category
         << ", Description: " << description << endl;
}

// Equality compares all shared fields
bool Transaction::operator==(const Transaction& other) const {
    return transactionID == other.transactionID
        && date == other.date
        && amount == other.amount
        && category == other.category
        && description == other.description;
}

// Friend function writing a transaction's fields to the given stream
ostream& operator<<(ostream& os, const Transaction& t) {
    os << "ID: " << t.transactionID
       << ", Date: " << t.date
       << ", Amount: " << t.amount
       << ", Category: " << t.category
       << ", Description: " << t.description;
    return os;
}


std::unique_ptr<Transaction> Transaction::fromJSON(const json& json) {
    const auto type = json.at("__type");
    if (!type.is_number()) {
        throw invalid_argument("Invalid type. Must be a number.");
    }

    if (1 == type) {
        return Expense::fromJSON(json);
    }

    if (2 == type) {
        return Income::fromJSON(json);
    }

    return nullptr;
}


json Transaction::toJSON() const {
    json json;

    json.at("transactionId") = this->transactionID;
    json.at("date") = this->date;
    json.at("amount") = this->amount;
    json.at("category") = this->category;
    json.at("description") = this->description;

    return json;
}