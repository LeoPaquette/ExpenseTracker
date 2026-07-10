#include "include/TransactionManager.h"

#include <algorithm>

#include "include/Expense.h"
#include "include/Income.h"

#define assert_template_derives_transaction() \
    static_assert( \
        std::is_base_of_v<Transaction, std::decay_t<T>>, \
        "Template argument must derive class Transaction" \
    )


TransactionManager::TransactionManager() {
    this->transactions = std::vector<std::unique_ptr<Transaction>>();
    this->categories = std::vector<std::unique_ptr<Category>>();
}


template <typename T>
void TransactionManager::addTransaction(const T& transaction) {
    assert_template_derives_transaction();

    this->transactions.push_back(
        std::make_unique<std::decay_t<T>>(std::forward<T>(transaction))
    );
}


template <typename T>
void TransactionManager::editTransaction(T& transaction) {
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

        return;
    }

    if (const auto storedPtr = dynamic_cast<Expense*>(storedTransaction.get())) {
        const auto providedPtr = dynamic_cast<Expense&>(transaction);

        storedPtr->setPaymentMethod(providedPtr.getPaymentMethod());

        return;
    }
}