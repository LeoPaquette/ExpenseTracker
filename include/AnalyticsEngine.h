#ifndef EXPENSETRACKER_ANALYTICSENGINE_H
#define EXPENSETRACKER_ANALYTICSENGINE_H

#include <functional>
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
    T computeTotalExpenses(const vector<reference_wrapper<const Transaction>>& transactions) const {
        T total{};
        for (const Transaction& t : transactions) {
            if (t.computeImpact()) {
                total += static_cast<T>(t.getAmount());
            }
        }
        return total;
    }

    T computeTotalIncome() const {

        return computeTotalIncome(T{});
    }
    T computeSavings() const {

        return computeSavings(T{});
    }
    void computeCategorySpending() const {

        return computeCategorySpending(T{});
    }
    void computeBudgetUsage() const {

        computeBudgetUsage(T{});
    }
};

#endif //EXPENSETRACKER_ANALYTICSENGINE_H
