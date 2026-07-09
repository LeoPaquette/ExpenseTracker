#ifndef EXPENSETRACKER_ANALYTICSENGINE_H
#define EXPENSETRACKER_ANALYTICSENGINE_H

// this template class calculates financial summaries
template <typename T = double>
class AnalyticsEngine {
public:

    /*
     * Find each unique transactionID from the Transaction.cpp class, grab each amount associated to the transactions,
     * then compute the total.
     */
    T computeTotalExpenses() const {


        return computeTotalExpenses(T{});
    }

    T computeTotalIncome() const {

        return computeTotalIncome(T{});
    }
    T computeSavings() const {

        return computeSavings(T{});
    }
    void computeCategorySpending() const {

        computeCategorySpending(T{});
    }
    void computeBudgetUsage() const {

        computeBudgetUsage(T{});
    }
};

#endif //EXPENSETRACKER_ANALYTICSENGINE_H
