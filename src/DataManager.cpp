#include "include/DataManager.h"

#include <fstream>
#include <iomanip>

#include "include/JSON.h"

std::optional<DataManager::DataReadWriteError> DataManager::loadData(
        std::vector<std::unique_ptr<Transaction>>& outTransactions,
        std::vector<std::unique_ptr<Category>>& outCategories
) const {
    std::ifstream transactionsFile(this->transactionFilePath);
    if (!transactionsFile.is_open()) {
        return DataReadWriteError::NO_SUCH_TRANSACTION_FILE;
    }

    std::ifstream categoriesFile(this->categoriesFilePath);
    if (!categoriesFile.is_open()) {
        transactionsFile.close();
        return DataReadWriteError::NO_SUCH_CATEGORY_FILE;
    }

    outTransactions.clear();
    json transactionsJson;
    transactionsFile >> transactionsJson;
    for (int i = 0; i < transactionsJson.size(); i++) {
        outTransactions.push_back(Transaction::fromJSON(transactionsJson.at(i)));
    }

    outCategories.clear();
    json categoriesJson;
    categoriesFile >> categoriesJson;
    for (int i = 0; i < categoriesJson.size(); i++) {
        outCategories.push_back(Category::fromJSON(categoriesJson.at(i)));
    }

    transactionsFile.close();
    categoriesFile.close();

    return std::nullopt;
}


std::optional<DataManager::DataReadWriteError> DataManager::saveData(
         const std::vector<std::unique_ptr<Transaction>>& transactions,
         const std::vector<std::unique_ptr<Category>>& categories
) const {
    std::ofstream transactionsFile(this->transactionFilePath);
    if (!transactionsFile.is_open()) {
        return DataReadWriteError::CANNOT_OPEN_FILE;
    }

    std::ofstream categoriesFile(this->categoriesFilePath);
    if (!categoriesFile.is_open()) {
        transactionsFile.close();
        return DataReadWriteError::CANNOT_OPEN_FILE;
    }

    json transactionsJson;
    for (int i = 0; i < transactions.size(); i++) {
        transactionsJson.at(i) = transactions.at(i)->toJSON();
    }

    json categoriesJson;
    for (int i = 0; i < categories.size(); i++) {
        categoriesJson.at(i) = categories.at(i)->toJSON();
    }

    transactionsFile << std::setw(4) << transactionsJson << std::endl;
    categoriesFile << std::setw(4) << categoriesJson << std::endl;

    transactionsFile.close();
    categoriesFile.close();

    return std::nullopt;
}