#ifndef EXPENSETRACKER_ANALYTICSENGINE_H
#define EXPENSETRACKER_ANALYTICSENGINE_H

#include <vector>

#include "Transaction.h"

using namespace std;

// this template class calculates financial summaries
template <typename T = double>
class AnalyticsEngine {
public:

    /**
     *  @brief adds up every expense in the list and returns the total.
     *
     * anything where computeImpact() is true counts as an expense.
     * feed it whatever searchTransactions() or filterByDateRange() gives back.
     */
    T computeTotalExpenses(const std::vector<const Transaction*>& transactions) const {
        T total{};
        for (const Transaction* t : transactions) {
            if (t->computeImpact()) {
                total += (t->getAmount());
            }
        }
        return total;
    }

    T computeTotalIncome(const std::vector<const Transaction*>& transactions) const {
        T total{};
        for (const Transaction* t : transactions) {
            if (!t->computeImpact()) {
                total += (t->getAmount());
            }
        }
        return total;
    }

    T computeSavings(const std::vector<const Transaction*>& transactions) const {
    return computeTotalIncome(transactions) - computeTotalExpenses(transactions);
    }

    std::map<std::string, T> computeCategorySpending(const std::vector<const Transaction*>& transactions) const {
        std::map<std::string, T> spending;
        for (const Transaction* t : transactions) {
            if (t->computeImpact()) {
                spending[t->getCategory()] += static_cast<T>(t->getAmount());
            }
        }
        return spending;
    }
    void computeBudgetUsage() const {

        computeBudgetUsage(T{});
    }
};

#endif //EXPENSETRACKER_ANALYTICSENGINE_H