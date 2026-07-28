#ifndef EXPENSETRACKER_GUI_H
#define EXPENSETRACKER_GUI_H

#include <QWidget>
#include <QLineEdit>

#include "include/TransactionManager.h"
#include "include/AnalyticsEngine.h"

// GUI.cpp needs the generated header
QT_BEGIN_NAMESPACE
namespace Ui {
    class ExpenseTrackerWindow;
}
QT_END_NAMESPACE

// main application window
class GUI : public QWidget {
    Q_OBJECT

public:
    // constructor building the window from the Designer layout
    explicit GUI(QWidget* parent = nullptr);
    // destructor releasing the generated UI
    ~GUI() override;

private slots:
    // jumps to the transaction form tab
    void onAddTransactionClicked() const;
    // returns to the main tab WITHOUT saving
    void onCancelTransactionClicked() const;
    // relabels the extra field to match the selected transaction type
    void onTypeChanged(const QString& type) const;
    // wiring the Browse files button for Transactions in the main tab
    void onBrowseTransactionsClicked();
    // wiring the Browse files button for Categories in the main tab
    void onBrowseCategoriesClicked();
    // recalculates the analytics tab from the currently loaded data
    void onRefreshAnalyticsClicked();
    // reads both data files listed on the main tab into the transaction manager
    void onLoadDataClicked();

private:
    // fills the main tab table from everything currently loaded
    void refreshTransactionsTable();

    Ui::ExpenseTrackerWindow* ui;
    TransactionManager transactionManager;
    AnalyticsEngine<> analyticsEngine;
};

#endif //EXPENSETRACKER_GUI_H