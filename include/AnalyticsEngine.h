#ifndef EXPENSETRACKER_ANALYTICSENGINE_H
#define EXPENSETRACKER_ANALYTICSENGINE_H

#include <vector>
#include <map>
#include "Transaction.h"
#include "Category.h"

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

    struct BudgetUsage {
        std::string categoryName;
        T budget;
        T spent;
        T usagePercent;
    };

    std::vector<BudgetUsage> computeBudgetUsage(const std::vector<const Transaction*>& transactions,
        const std::vector<const Category*>& categories) const {
        const std::map<std::string, T> spending = computeCategorySpending(transactions);

        std::vector<BudgetUsage> rows;
        rows.reserve(categories.size());

        for (const Category* c : categories) {
            BudgetUsage row;
            row.categoryName = c->getName();
            row.budget = static_cast<T>(c->getMonthlyBudget());

            row.spent = spentIn(spending, row.categoryName);
            row.usagePercent = percentOf(row.spent, row.budget);

            rows.push_back(row);
        }

        return rows;
    }

private:
    // returns the amount spent in a category, or zero if it has no expenses.
    T spentIn(const std::map<std::string, T>& spending, const std::string& categoryName) const {
        const auto it = spending.find(categoryName);
        return (it != spending.end()) ? it->second : T{};
    }

    // returns spent as a percentage of budget, or zero when the budget is zero.
    static T percentOf(T spent, T budget) {
        return (budget > T{}) ? (spent / budget) * static_cast<T>(100) : T{};
    }
};

#endif //EXPENSETRACKER_ANALYTICSENGINE_H