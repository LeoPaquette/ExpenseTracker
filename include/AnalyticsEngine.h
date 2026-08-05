#ifndef EXPENSETRACKER_ANALYTICSENGINE_H
#define EXPENSETRACKER_ANALYTICSENGINE_H

#include <vector>
#include <map>
#include "Transaction.h"
#include "Category.h"

/**
 * @brief Calculates the financial summaries shown on the analytics tab.
 *
 * Holds no state of its own. Every method takes the transactions to work on as an argument, so the
 * same engine can summarise everything loaded or just the results of a search.
 *
 * @tparam T The arithmetic type totals are accumulated and returned in. Defaults to @c double.
 */
template <typename T = double>
class AnalyticsEngine {
public:

    /**
     *  @brief adds up every expense in the list and returns the total.
     *
     * anything where computeImpact() is true counts as an expense.
     * feed it whatever searchTransactions() or filterByDateRange() gives back.
     *
     * @param transactions The transactions to total.
     *
     * @return The sum of every expense, or zero if there are none.
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

    /**
     * @brief Adds up every income entry in the list and returns the total.
     *
     * The mirror of computeTotalExpenses(): anything where computeImpact() is false counts as income.
     *
     * @param transactions The transactions to total.
     *
     * @return The sum of every income entry, or zero if there are none.
     */
    T computeTotalIncome(const std::vector<const Transaction*>& transactions) const {
        T total{};
        for (const Transaction* t : transactions) {
            if (!t->computeImpact()) {
                total += (t->getAmount());
            }
        }
        return total;
    }

    /**
     * @brief Returns what is left over once expenses are taken out of income.
     *
     * @param transactions The transactions to summarise.
     *
     * @return Total income minus total expenses. Negative means more was spent than earned.
     */
    T computeSavings(const std::vector<const Transaction*>& transactions) const {
    return computeTotalIncome(transactions) - computeTotalExpenses(transactions);
    }

    /**
     * @brief Totals the expenses in each category.
     *
     * Income is ignored. Categories with no expenses simply do not appear in the result, so callers
     * should treat a missing key as zero rather than assuming every category is present.
     *
     * @param transactions The transactions to group.
     *
     * @return A map of category name to the amount spent in it.
     */
    std::map<std::string, T> computeCategorySpending(const std::vector<const Transaction*>& transactions) const {
        std::map<std::string, T> spending;
        for (const Transaction* t : transactions) {
            if (t->computeImpact()) {
                spending[t->getCategory()] += static_cast<T>(t->getAmount());
            }
        }
        return spending;
    }

    /**
     * @brief One category's budget against what has actually been spent in it.
     */
    struct BudgetUsage {
        /** @brief The name of the category this row describes. */
        std::string categoryName;

        /** @brief The monthly budget set for the category. */
        T budget;

        /** @brief The total spent in the category, or zero if nothing was. */
        T spent;

        /** @brief Spent as a percentage of budget, or zero when the category has no budget. */
        T usagePercent;
    };

    /**
     * @brief Builds one BudgetUsage row per category, in the order the categories are given.
     *
     * Every category gets a row whether or not anything was spent in it, so the analytics table can
     * show the full list rather than only the categories in use.
     *
     * @param transactions The transactions to measure the budgets against.
     * @param categories   The categories to report on.
     *
     * @return One row per category, ready to display.
     */
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