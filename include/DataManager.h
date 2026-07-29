#ifndef EXPENSETRACKER_DATAMANAGER_H
#define EXPENSETRACKER_DATAMANAGER_H

#include <string>
#include <memory>
#include <optional>
#include <vector>

#include "include/Category.h"
#include "include/Transaction.h"

/**
 * @brief Persists transactions and categories to and from JSON files.
 *
 * Reads and writes the two configured files (one for transactions, one for categories),
 * serializing each record via its @c toJSON / @c fromJSON methods.
 */
class DataManager {
    const std::string transactionFilePath;
    const std::string categoriesFilePath;

public:
    /**
     * @brief The errors which can occur while reading or writing the data files.
     */
    enum class DataReadWriteError {
        /** @brief Indicates an unknown, unexpected internal error. */
        UNKNOWN_ERROR,

        /** @brief Indicates that one of the save files could not be opened */
        CANNOT_OPEN_FILE,

        /** @brief Indicates on read, that the file specified for transactions does not exist. */
        NO_SUCH_TRANSACTION_FILE,

        /** @brief Indicates on read, that the file specified for categories does not exist. */
        NO_SUCH_CATEGORY_FILE,
    };

    /**
     * @brief Constructs a data manager bound to the given file paths.
     *
     * @param transactionFilePath The path to the JSON file used for transactions.
     * @param categoriesFilePath  The path to the JSON file used for categories.
     */
    DataManager(const std::string& transactionFilePath, const std::string& categoriesFilePath)
        : transactionFilePath(transactionFilePath), categoriesFilePath(categoriesFilePath) {}

    /**
     * @brief Loads transactions and categories from the configured files.
     *
     * Both output vectors are cleared and repopulated from the parsed JSON. On failure the
     * outputs are left unchanged.
     *
     * @param outTransactions Destination vector for the loaded transactions.
     * @param outCategories   Destination vector for the loaded categories.
     *
     * @return @c std::nullopt on success, or a @c DataReadWriteError describing the failure.
     */
    std::optional<DataReadWriteError> loadData(
        std::vector<std::unique_ptr<Transaction>>& outTransactions,
        std::vector<std::unique_ptr<Category>>& outCategories
    ) const;

    /**
     * @brief Saves the given transactions and categories to the configured files.
     *
     * Each record is serialized via its @c toJSON method and written as pretty-printed JSON.
     *
     * @param transactions The transactions to write.
     * @param categories   The categories to write.
     *
     * @return @c std::nullopt on success, or a @c DataReadWriteError describing the failure.
     */
    std::optional<DataReadWriteError> saveData(
        const std::vector<std::unique_ptr<Transaction>>& transactions,
        const std::vector<std::unique_ptr<Category>>& categories
    ) const;
};

#endif //EXPENSETRACKER_DATAMANAGER_H
