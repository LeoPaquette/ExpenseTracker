#ifndef EXPENSETRACKER_TRANSACTIONMANAGER_H
#define EXPENSETRACKER_TRANSACTIONMANAGER_H

#include <expected>
#include <memory>
#include <optional>
#include <vector>

#include "Category.h"
#include "Transaction.h"

using namespace std;

class TransactionManager {
    vector<unique_ptr<Transaction>> transactions;
    vector<unique_ptr<Category>> categories;

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
    ~TransactionManager();
    TransactionManager(const TransactionManager&);
    TransactionManager& operator=(const TransactionManager&);

    /**
     * @brief Stores a copy of the provided transaction in memory.
     *
     * @param transaction The transaction to store.
     */
    void addTransaction(const Transaction& transaction);

    /**
     * @brief Updates the provided transaction based on its ID.
     *
     * @param transaction The transaction to update.
     */
    void editTransaction(Transaction& transaction);

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
    expected<vector<reference_wrapper<const Transaction>>, SearchError> searchTransactions(
        optional<const string&> date,
        optional<const Category&> category,
        optional<double> amount
    );

    /**
     * @brief Returns the stored transactions from the provided category.
     *
     * @param category The category of transactions to retrieve.
     *
     * @return A vector of const references to the matching transactions, or empty if nothing matched.
     */
    vector<reference_wrapper<const Transaction>> filterByCategory(const Category& category);

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
    vector<reference_wrapper<const Transaction>> filterByDateRange(
        optional<const string&> startDate,
        optional<const string&> endDate
    );
};

#endif //EXPENSETRACKER_TRANSACTIONMANAGER_H
