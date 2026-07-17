#ifndef EXPENSETRACKER_DATAMANAGER_H
#define EXPENSETRACKER_DATAMANAGER_H

#include <string>
#include <memory>
#include <optional>
#include <vector>

#include "include/Category.h"
#include "include/Transaction.h"

class DataManager {
    const std::string transactionFilePath;
    const std::string categoriesFilePath;

public:
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

    DataManager(const std::string &transactionFilePath, const std::string &categoriesFilePath)
        : transactionFilePath(transactionFilePath), categoriesFilePath(categoriesFilePath) {}

    std::optional<DataReadWriteError> loadData(
        std::vector<std::unique_ptr<Transaction>>& outTransactions,
        std::vector<std::unique_ptr<Category>>& outCategories
    ) const;

    std::optional<DataReadWriteError> saveData(
        const std::vector<std::unique_ptr<Transaction>>& transactions,
        const std::vector<std::unique_ptr<Category>>& categories
    ) const;
};

#endif //EXPENSETRACKER_DATAMANAGER_H
