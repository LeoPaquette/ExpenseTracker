#ifndef EXPENSETRACKER_DATAMANAGER_H
#define EXPENSETRACKER_DATAMANAGER_H

#include <expected>
#include <string>
#include <memory>
#include <optional>
#include <vector>

#include "Category.h"
#include "Transaction.h"

class DataManager {
    std::string transactionFilePath;
    std::string categoriesFilePath;

public:
    enum class DataReadWriteError {
        /** @brief Indicates an unknown, unexpected internal error. */
        UNKNOWN_ERROR,

        /** @brief Indicates on read, that the file specified for transactions does not exist. */
        NO_SUCH_TRANSACTION_FILE,

        /** @brief Indicates on read, that the file specified for categories does not exist. */
        NO_SUCH_CATEGORY_FILE,
    };

    DataManager(std::string transactionFilePath, std::string categoriesFilePath);

    std::optional<DataReadWriteError> saveData(
        std::vector<std::unique_ptr<Transaction>> transactions,
        std::vector<std::unique_ptr<Category>> categories
    );

    std::optional<DataReadWriteError> loadData(
        std::vector<std::unique_ptr<Transaction>>* outTransactions,
        std::vector<std::unique_ptr<Category>>* outCategories
    );
};

#endif //EXPENSETRACKER_DATAMANAGER_H
