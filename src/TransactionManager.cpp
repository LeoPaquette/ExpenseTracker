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
        static const std::regex pattern(R"(^(\d{4})-(\d{2})-(\d{2})$)");

        if (smatch match; !std::regex_match(*date.value(), match, pattern)) {
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


inline std::expected<
        std::vector<const Transaction*>,
        TransactionManager::SearchError
> TransactionManager::getAllTransactions() const {
    return this->searchTransactions(std::nullopt, std::nullopt, std::nullopt);
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
    std::optional<const string*> startDate,
    std::optional<const string*> endDate
) const {
    throw std::runtime_error("fuck off");
}