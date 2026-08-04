#ifndef EXPENSETRACKER_TRANSACTIONMANAGER_H
#define EXPENSETRACKER_TRANSACTIONMANAGER_H

#include <algorithm>
#include <expected>
#include <memory>
#include <optional>
#include <ranges>
#include <vector>

#include "Expense.h"
#include "Income.h"
#include "include/Category.h"
#include "include/DataManager.h"
#include "include/Transaction.h"



#define assert_template_derives_transaction() \
    static_assert( \
        std::is_base_of_v<Transaction, std::decay_t<T>>, \
        "Template argument must derive class Transaction" \
    )

/**
 * @brief Owns and manages the application's transactions and categories.
 *
 * Acts as the central in-memory store for the expense tracker. It provides operations to add,
 * edit, delete, search and filter transactions and categories, and delegates persistence to a
 * @c DataManager via @c load and @c save.
 */
class TransactionManager {
    /**
     * TODO: Document
     */
    std::vector<unsigned int> usedCategoryIds = std::vector<unsigned int>();

    /**
     * TODO: Document
     */
    unsigned int nextCategoryId = 0;

    /**
     * @brief The transactions currently held in memory, each uniquely owned.
     */
    std::vector<std::unique_ptr<Transaction>> transactions;

    /**
     * @brief The categories currently held in memory, each uniquely owned.
     */
    std::vector<std::unique_ptr<Category>> categories;

    /**
     * @brief Finds a stored category by its ID.
     *
     * @param id The ID of the category to find.
     *
     * @return A non-owning pointer to the matching category, or @c nullptr if none matched.
     */
    Category* tryGetCategoryById(const string& id);

    /**
     * @brief Finds a stored category by its name.
     *
     * @param name The name of the category to find.
     *
     * @return A non-owning pointer to the matching category, or @c nullptr if none matched.
     */
    Category* tryGetCategoryByName(const string& name);

    /**
     * @brief Finds a stored transaction by its ID.
     *
     * @param id The ID of the transaction to find.
     *
     * @return A non-owning pointer to the matching transaction, or @c nullptr if none matched.
     */
    Transaction* tryGetTransactionById(const string& id);

    /**
     * @brief Finds the first stored transaction belonging to the given category.
     *
     * The category name is compared case-insensitively.
     *
     * @param category The category name to match against.
     *
     * @return A non-owning pointer to the first matching transaction, or @c nullptr if none matched.
     */
    Transaction* tryGetFirstTransactionByCategory(const string& category);

public:
    /**
     * @brief The errors which can occur during a search
     */
    enum class SearchError {
        /** @brief Indicates an unknown, unexpected internal error. */
        UNKNOWN_ERROR,

        /** @brief Indicates that the provided date filter is not in the correct format. */
        INVALID_DATE_FORMAT,

        /** @brief Indicates that the provided date filter is in the future. */
        INVALID_DATE_FUTURE,

        /** @brief Indicates that the provided amount filter exceeds the maximum value. */
        INVALID_AMOUNT_EXCEEDS_MAX,

        /** @brief Indicates that the provided amount filter is a negative number. */
        INVALID_AMOUNT_NEGATIVE,
    };

    /**
     * @brief Constructs an empty manager with no transactions or categories.
     */
    TransactionManager();

    /**
     * @brief Destroys the manager, releasing all owned transactions and categories.
     */
    ~TransactionManager() = default;

    /**
     * @brief Copy-constructs a manager from another (compiler-generated member-wise copy).
     */
    TransactionManager(const TransactionManager&) = default;

    /**
     * @brief Copy-assigns from another manager (compiler-generated member-wise copy).
     */
    TransactionManager& operator=(const TransactionManager&) = default;

    /**
     * TODO: Document
     */
    unsigned int getNextCategoryId();

    /**
     * @brief Stores a copy of the provided category in memory.
     *
     * @param category The category to store.
     */
    void addCategory(const Category& category);

    /**
     * @brief Updates the provided category based on its ID.
     *
     * @param category The category to update.
     */
    void editCategory(const Category& category);

    /**
     * @brief Deletes the category with the provided ID.
     *
     * @param categoryId The category to update.
     */
    void deleteCategory(const string& categoryId);

    /**
     * @brief Stores a copy of the provided transaction in memory.
     *
     * @param transaction The transaction to store.
     */
    template <typename T>
    void addTransaction(const T& transaction) {
        assert_template_derives_transaction();

        this->transactions.push_back(
            std::make_unique<std::decay_t<T>>(transaction)
        );
    }

    /**
     * @brief Updates the provided transaction based on its ID.
     *
     * @param transaction The transaction to update.
     */
    template <typename T>
    void editTransaction(const T& transaction) {
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
            const auto providedPtr = dynamic_cast<const Income&>(transaction);

            storedPtr->setSource(providedPtr.getSource());

            return;
        }

        if (const auto storedPtr = dynamic_cast<Expense*>(storedTransaction.get())) {
            const auto providedPtr = dynamic_cast<const Expense&>(transaction);

            storedPtr->setPaymentMethod(providedPtr.getPaymentMethod());

            return;
        }

        throw std::runtime_error("Transaction type handling unimplemented!");
    }

    /**
     * @brief Deletes the transaction with the provided ID.
     *
     * @param transactionId The transaction to update.
     */
    void deleteTransaction(const string& transactionId);

    /**
     * @brief Returns all stored transactions that match every supplied filter.
     *
     * Each argument is an optional filter. Supplied filters are combined with a logical AND
     * operation, meaning a transaction is included only if it matches all criteria filters.
     *
     * Arguments left as @c std::nullopt are ignored.
     *
     * If no filters are supplied, all stored transactions are returned.
     *
     * Inputs are validated before any matching occurs; an invalid argument causes the whole call
     * to fail with the corresponding SearchError.
     *
     * @param date      If present, matches transactions on this calendar date.
     * @param category  If present, matches transactions in this category.
     * @param amount    If present, matches transactions with exactly this amount.
     *
     * @return  On success, a vector of const references to the matching transactions in insertion
     *          order (empty if nothing matched).
     *          <p>
     *          On failure, one @c SearchError describing the first rejected input.
     */
    std::expected<
        std::vector<const Transaction*>,
        SearchError
    > searchTransactions(
        std::optional<const string*> date,
        std::optional<const Category*> category,
        std::optional<double> amount
    ) const;

    /**
     * Shorthand for a call to @c searchTransactions with no provided filters
     */
    inline std::expected<
        std::vector<const Transaction*>,
        SearchError
    > getAllTransactions() const {
        return this->searchTransactions(std::nullopt, std::nullopt, std::nullopt);
    }

    /**
     * Returns a vector of all known Categories.
     */
    std::vector<const Category*> getAllCategories() const;

    /**
     * @brief Returns the stored transactions from the provided category.
     *
     * @param category The category of transactions to retrieve.
     *
     * @return A vector of const references to the matching transactions, or empty if nothing matched.
     */
    std::vector<const Transaction*> filterByCategory(const Category& category) const;

    /**
     * @brief Returns the stored transactions that are within the provided range inclusive.
     *
     * If no @p startDate is provided, all transactions before @p endDate are returned.
     *
     * If no @p endDate is provided, all transactions after @p startDate are returned.
     *
     * If no @p startDate and @p endDate are provided, all transactions are returned.
     *
     * @param startDate The category of transactions to retrieve.
     * @param endDate The category of transactions to retrieve.
     *
     * @return A vector of const references to the matching transactions, or empty if nothing matched.
     */
    std::vector<const Transaction*> filterByDateRange(
        std::optional<const string*> startDate,
        std::optional<const string*> endDate
    ) const;

    /**
     * Uses the provided data manager to load data into the transaction manager.
     *
     * @param dataManager The instance of data manager to use to load data.
     *
     * @return Returns either the total number of records loaded, or an error.
     */
    std::expected<int, DataManager::DataReadWriteError> load(const DataManager& dataManager);

    /**
     * Uses the provided data manager to save data into the transaction manager.
     *
     * @param dataManager The instance of data manager to use to save data.
     *
     * @return Returns either the total number of records saved, or an error.
     */
    std::expected<int, DataManager::DataReadWriteError> save(const DataManager& dataManager) const;
};

#endif //EXPENSETRACKER_TRANSACTIONMANAGER_H
