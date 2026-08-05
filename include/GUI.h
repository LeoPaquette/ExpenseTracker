#ifndef EXPENSETRACKER_GUI_H
#define EXPENSETRACKER_GUI_H

#include <QWidget>
#include <QLineEdit>
#include <QCloseEvent>

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

protected:
    // asks the user what to do about unsaved changes before the window actually closes
    void closeEvent(QCloseEvent* event) override;

private slots:
    // jumps to the transaction form tab with a blank form
    void onAddTransactionClicked();
    // returns to the main tab WITHOUT saving
    void onCancelTransactionClicked();
    // builds a transaction from the form and either adds it or updates the one being edited
    void onSaveTransactionClicked();
    // loads whichever transaction is selected in the main tab table into the form for editing
    void onEditTransactionClicked();
    // builds a category from the category form and adds it
    void onSaveCategoryClicked();
    // recalculates the analytics tab whenever the user switches to it
    void onTabChanged(int index);
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
    // writes everything currently in memory out to both data files listed on the main tab
    void onSaveDataClicked();
    // returns to the main tab WITHOUT saving the category
    void onCancelCategoryClicked() const;
    // empties every search/filter input on the main tab and restores the full table
    void onClearFiltersClicked();
    // filters the main tab table down to the transactions matching the filled in filters
    void onSearchTransactionsClicked();
    // deletes whichever transaction is selected in the main tab table
    void onDeleteTransactionClicked();

private:
    // fills the main tab table from everything currently loaded
    void refreshTransactionsTable();
    // fills the main tab table with whichever transactions it gets handed
    void populateTransactionsTable(const std::vector<const Transaction*>& transactions) const;
    // fills the category tab table from everything currently loaded
    void refreshCategoriesTable();
    // resets the transaction form back to a blank add-mode state
    void clearTransactionForm();
    // resets the category form back to its default state
    void clearCategoryForm() const;
    // reads both data files listed on the main tab, optionally announcing how many records arrived
    void loadDataFiles(bool showSuccessMessage);
    // stores the current file paths so the next launch can reopen them
    void rememberDataFilePaths() const;

    Ui::ExpenseTrackerWindow* ui;
    TransactionManager transactionManager;
    AnalyticsEngine<> analyticsEngine;

    // empty while adding a new transaction, holds the target id while editing an existing one
    QString editingTransactionId;
    // tracks whether memory has moved ahead of the data files, so closing can warn about it
    bool hasUnsavedChanges = false;
};

#endif //EXPENSETRACKER_GUI_H