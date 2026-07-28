#include "include/TransactionManager.h"

#include <algorithm>
#include <ranges>
#include <regex>

#include "include/Expense.h"
#include "include/Income.h"

#define assert_template_derives_transaction() \
    static_assert( \
        std::is_base_of_v<Transaction, std::decay_t<T>>, \
        "Template argument must derive class Transaction" \
    )


static const regex DATE_PATTERN(R"(^(\d{4})-(\d{2})-(\d{2})$)");
static const int DAYS_IN_MONTH[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static constexpr bool isYearLeapYear(const int year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

static bool parseDate(const std::string& value, int* outYear, int* outMonth, int* outDay) {
    smatch match;
    if (value.empty() || !regex_match(value, match, DATE_PATTERN)) {
        return false;
    }

    const int year = stoi(match[1].str());
    const int month = stoi(match[2].str());
    const int day = stoi(match[3].str());

    int maxDays = DAYS_IN_MONTH[month - 1];
    if (2 == month && isYearLeapYear(year)) {
        maxDays = 29;
    }

    if (1 > day || day > maxDays) {
        return false;
    }

    *outYear = year;
    *outMonth = month;
    *outDay = day;
    return true;
}

static int compareDates(const std::string& a, const std::string& b) {
    int aYear = 0;
    int aMonth = 0;
    int aDay = 0;
    if (!parseDate(a, &aYear, &aMonth, &aDay)) {
        throw std::runtime_error("a is not a valid date");
    }

    int bYear = 0;
    int bMonth = 0;
    int bDay = 0;
    if (!parseDate(b, &bYear, &bMonth, &bDay)) {
        throw std::runtime_error("b is not a valid date");
    }

    if (aYear < bYear) {
        return -1;
    }

    if (aYear > bYear) {
        return 1;
    }

    if (aMonth < bMonth) {
        return -1;
    }

    if (aMonth > bMonth) {
        return 1;
    }

    if (aDay < bDay) {
        return -1;
    }

    if (aDay > bDay) {
        return 1;
    }

    return 0;
}

static bool insensitive_equals(std::string a, std::string b) {
    return std::ranges::equal(
        a,
        b,
        [](const auto c1, const auto c2) {
            return std::tolower(c1) == std::tolower(c2);
        }
    );
}


void TransactionManager::updateStoredCategories() {
    std::erase_if(this->categories, [this](const auto& c) {
        /* check if no transaction has the current category */
        return nullptr == this->tryGetFirstTransactionByCategory(c->getName());
    });
}


Category* TransactionManager::tryGetCategoryById(const string& id) {
    /* Find matching element */
    const auto elem = std::ranges::find_if(
        this->categories,
        [id](const auto& c) {
            return c && id == c->getCategoryID();
        }
    );


    /* If no element matches */
    if (elem == this->categories.end()) {
        return nullptr;
    }

    return elem->get();
}


Category* TransactionManager::tryGetCategoryByName(const string& name) {
    /* Find matching element */
    const auto elem = std::ranges::find_if(
        this->categories,
        [name](const auto& c) {
            return c && name == c->getName();
        }
    );


    /* If no element matches */
    if (elem == this->categories.end()) {
        return nullptr;
    }

    return elem->get();
}


Transaction* TransactionManager::tryGetTransactionById(const string& id) {
    /* Find matching element */
    const auto elem = std::ranges::find_if(
        this->transactions,
        [id](const auto& t) {
            return t && id == t->getTransactionID();
        }
    );


    /* If no element matches */
    if (elem == this->transactions.end()) {
        return nullptr;
    }

    return elem->get();
}


Transaction* TransactionManager::tryGetFirstTransactionByCategory(
    const string& category
) {
    /* Find matching element */
    const auto elem = std::ranges::find_if(
        this->transactions,
        [category](const auto& t) {
            return t && insensitive_equals(category, t->getCategory());
        }
    );


    /* If no element matches */
    if (elem == this->transactions.end()) {
        return nullptr;
    }

    return elem->get();
}



TransactionManager::TransactionManager() {
    this->transactions = std::vector<std::unique_ptr<Transaction>>();
    this->categories = std::vector<std::unique_ptr<Category>>();
}


void TransactionManager::addCategory(const Category& category) {
    /* check for duped id */
    if (nullptr != this->tryGetCategoryById(category.getCategoryID())) {
        return;
    }

    /* check for duped name */
    if (nullptr != this->tryGetCategoryByName(category.getName())) {
        return;
    }

    this->categories.push_back(std::make_unique<std::decay_t<Category>>(category));
}


void TransactionManager::editCategory(const Category& category) {
    /* Find matching category */
    const auto stored = this->tryGetCategoryById(category.getCategoryID());


    /* If no transaction matches, add it */
    if (nullptr == stored) {
        this->addCategory(category);
        return;
    }


    /* Update the stored data */
    stored->setName(category.getName());
    stored->setMonthlyBudget(category.getMonthlyBudget());
}


void TransactionManager::deleteCategory(const string& categoryId) {
    std::erase_if(
        this->categories,
        [categoryId](const auto& c) {
            return c && c->getCategoryID() == categoryId;
        }
    );
}


template <typename T>
void TransactionManager::addTransaction(const T& transaction) {
    assert_template_derives_transaction();

    this->transactions.push_back(
        std::make_unique<std::decay_t<T>>(std::forward<T>(transaction))
    );
}


template <typename T>
void TransactionManager::editTransaction(const T& transaction) {
    assert_template_derives_transaction();

    const string targetTransactionId = transaction.getTransactionID();

    /* Find matching transaction */
    auto iter = std::find_if(
        this->transactions.begin(),
        this->transactions.end(),
        [&targetTransactionId](const std::unique_ptr<Transaction>& t) {
            return targetTransactionId == t->getTransactionID();
        }
    );


    /* If no transaction matches, add it */
    if (iter == this->transactions.end()) {
        this->addTransaction(transaction);
        return;
    }

    const std::unique_ptr<Transaction>& storedTransaction = *iter;


    /* Check if the type of the provided item matches the stored one */
    if (const auto& stored = *storedTransaction; typeid(stored) != typeid(transaction)) {
        throw std::invalid_argument("Type mismatch when editing transaction");
    }


    /* Update the stored data */
    storedTransaction->setDate(transaction.getDate());
    storedTransaction->setAmount(transaction.getAmount());
    storedTransaction->setCategory(transaction.getCategory());
    storedTransaction->setDescription(transaction.getDescription());

    if (const auto storedPtr = dynamic_cast<Income*>(storedTransaction.get())) {
        const auto providedPtr = dynamic_cast<Income&>(transaction);

        storedPtr->setSource(providedPtr.getSource());

        goto update_categories_and_return;
    }

    if (const auto storedPtr = dynamic_cast<Expense*>(storedTransaction.get())) {
        const auto providedPtr = dynamic_cast<Expense&>(transaction);

        storedPtr->setPaymentMethod(providedPtr.getPaymentMethod());

        goto update_categories_and_return;
    }

    throw std::runtime_error("Transaction type handling unimplemented!");

update_categories_and_return:
    this->updateStoredCategories();
}


void TransactionManager::deleteTransaction(const string& transactionId) {
    const auto count = std::erase_if(
        this->transactions,
        [transactionId](const auto& t) {
            return t && t->getTransactionID() == transactionId;
        }
    );

    if (0 < count) {
        this->updateStoredCategories();
    }
}


std::expected<
    std::vector<const Transaction*>,
    TransactionManager::SearchError
> TransactionManager::searchTransactions(
    const std::optional<const string*> date,
    const std::optional<const Category*> category,
    const std::optional<double> amount
) const {
    if (date.has_value() && Transaction::isValidDate(*date.value())) {
        if (smatch match; !std::regex_match(*date.value(), match, DATE_PATTERN)) {
            return std::unexpected(SearchError::INVALID_DATE_FORMAT);
        }

        return std::unexpected(SearchError::INVALID_DATE_FUTURE);
    }

    if (amount.has_value() && 0 > amount.value()) {
        return std::unexpected(SearchError::INVALID_AMOUNT_NEGATIVE);
    }

    if (amount.has_value() && 1000000.0 < amount.value()) {
        return std::unexpected(SearchError::INVALID_AMOUNT_EXCEEDS_MAX);
    }

    std::vector<const Transaction*> filtered;

    for (const auto& t : this->transactions) {
        const bool matches = t
                && (!date.has_value() || *date.value() == t->getDate())
                && (!category.has_value() || insensitive_equals(category.value()->getName(), t->getCategory()))
                && (!amount.has_value() || amount.value() == t->getAmount())
                ;

        if (matches) {
            filtered.push_back(t.get());
        }
    }

    return filtered;
}


std::vector<const Category*> TransactionManager::getAllCategories() const {
    std::vector<const Category*> out(this->categories.size());

    for (int i = 0; i < this->categories.size(); ++i) {
        out[i] = this->categories[i].get();
    }

    return out;
}


std::vector<const Transaction*> TransactionManager::filterByCategory(
    const Category& category
) const {
    std::vector<const Transaction*> filtered;

    for (const auto& t : this->transactions) {
        if (insensitive_equals(category.getName(), t->getCategory())) {
            filtered.push_back(t.get());
        }
    }

    return filtered;
}

std::vector<const Transaction*> TransactionManager::filterByDateRange(
    const std::optional<const string*> startDate,
    const std::optional<const string*> endDate
) const {
    std::vector<const Transaction*> filtered;

    if (!startDate.has_value() && !endDate.has_value()) {
        goto no_start_end;
    }

    if (!startDate.has_value() && endDate.has_value()) {
        goto no_start;
    }

    if (startDate.has_value() && !endDate.has_value()) {
        goto no_end;
    }

    throw std::runtime_error("Illegal state encountered!");

no_start:
    for (const auto& t : this->transactions) {
        if (1 != compareDates(t->getDate(), *endDate.value())) {
            filtered.push_back(t.get());
        }
    }

    goto exit;

no_end:
    for (const auto& t : this->transactions) {
        if (-1 != compareDates(t->getDate(), *startDate.value())) {
            filtered.push_back(t.get());
        }
    }

    goto exit;

no_start_end:
    for (const auto& t : this->transactions) {
        filtered.push_back(t.get());
    }

exit:
    return filtered;
}


std::expected<int, DataManager::DataReadWriteError> TransactionManager::load(const DataManager& dataManager) {
    if (
        const auto status = dataManager.loadData(transactions, categories);
        status.has_value()
    ) {
        return std::unexpected(status.value());
    }

    return transactions.size() + categories.size();
}


std::expected<int, DataManager::DataReadWriteError> TransactionManager::save(const DataManager& dataManager) const {
    if (
        const auto status = dataManager.saveData(transactions, categories);
        status.has_value()
    ) {
        return std::unexpected(status.value());
    }

    return transactions.size() + categories.size();
}
