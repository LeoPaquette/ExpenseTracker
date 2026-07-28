#ifndef EXPENSETRACKER_TRANSACTIONMANAGER_H
#define EXPENSETRACKER_TRANSACTIONMANAGER_H

#include <expected>
#include <memory>
#include <optional>
#include <ranges>
#include <vector>

#include "include/Category.h"
#include "include/DataManager.h"
#include "include/Transaction.h"

class TransactionManager {
    std::vector<std::unique_ptr<Transaction>> transactions;
    std::vector<std::unique_ptr<Category>> categories;

    /**
     * \brief   Updates the categories vector ensuring only those referenced
     *          by transactions are stored.
     */
    void updateStoredCategories();

    Category* tryGetCategoryById(const string& id);

    Category* tryGetCategoryByName(const string& name);

    Transaction* tryGetTransactionById(const string& id);

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

    TransactionManager();

    /*
     * No destructor, copy, or copy assignment needed, vectors
     * are handled by the compiler generated defaults
    */

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
    void addTransaction(const T& transaction);

    /**
     * @brief Updates the provided transaction based on its ID.
     *
     * @param transaction The transaction to update.
     */
    template <typename T>
    void editTransaction(const T& transaction);

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
