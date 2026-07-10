#ifndef EXPENSETRACKER_CATEGORY_H
#define EXPENSETRACKER_CATEGORY_H

class Category {
public:
    std::string getCategoryID() const;
    std::string getName() const;
    double getMonthlyBudget() const;

    void setName(std::string name);
    void setMonthlyBudget(double monthlyBudget);
};

#endif //EXPENSETRACKER_CATEGORY_H
