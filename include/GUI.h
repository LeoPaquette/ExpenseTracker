/*
* CST8219 - Personal Expense Tracker and Budget Management System
 * This file declares all the slots and required declarations for GUI.cpp
 * @author Wesley Deir
*/

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
    /**
     * @brief Asks the user what to do about unsaved changes before the window actually closes.
     *
     * Closes immediately when nothing is pending. Otherwise offers Save, Discard or Cancel, and
     * keeps the window open if the save itself fails.
     *
     * @param event The close request, accepted to let the window close or ignored to keep it open.
     */
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
    /**
     * @brief Recalculates the analytics tab whenever the user switches to it.
     *
     * @param index The index of the tab that just became current; ignored unless it is Analytics.
     */
    void onTabChanged(int index);

    /**
     * @brief Relabels the extra field to match the selected transaction type.
     *
     * @param type The newly selected type, either @c "Expense" or @c "Income".
     */
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
    /**
     * @brief Shows the full details of whichever transaction row was double clicked.
     *
     * @param row The double clicked row in the main tab table.
     */
    void onTransactionRowDoubleClicked(int row);
    /**
     * @brief Shows the budget summary of whichever category row was double clicked.
     *
     * @param row The double clicked row in the category tab table.
     */
    void onCategoryRowDoubleClicked(int row);

private:
    // fills the main tab table, keeping whatever filters the user has set
    void refreshTransactionsTable();
    /**
     * @brief Rebuilds the main tab table from whatever the filter widgets currently say.
     *
     * The one place the filters are read, so a save or a delete cant put every transaction back
     * into a table the user had narrowed down.
     *
     * @param reportErrors Whether an unusable filter raises a message box. The Search button passes
     *                     @c true, a rebuild after some other action passes @c false.
     *
     * @return @c true if the table was filled, @c false if a filter was unusable.
     */
    bool applyCurrentFilters(bool reportErrors);
    // empties the filter widgets on the main tab without touching the table
    void clearFilterInputs() const;
    /**
     * @brief Fills the main tab table with whichever transactions it gets handed.
     *
     * Shared by the full refresh and by a filtered search so both build rows the same way. The
     * amount cell also carries its raw value in @c Qt::UserRole, since its display text is money
     * formatted and the edit form needs the number back.
     *
     * @param transactions The transactions to show, in the order they should appear.
     */
    void populateTransactionsTable(const std::vector<const Transaction*>& transactions) const;
    // fills the category tab table from everything currently loaded
    void refreshCategoriesTable();
    // resets the transaction form back to a blank add-mode state
    void clearTransactionForm();
    // resets the category form back to its default state
    void clearCategoryForm() const;
    /**
     * @brief Reads both data files listed on the main tab into the transaction manager.
     *
     * The tables are rebuilt whether or not the read succeeded. @c TransactionManager::load restores
     * the previous data when it returns an error, but a malformed file throws out of it instead and
     * leaves the manager holding however much was parsed before the throw.
     *
     * @param showSuccessMessage Whether to report the record count on success. The Load Data button
     *                           passes @c true; the startup reload passes @c false so launching the
     *                           application does not open behind a message box.
     */
    void loadDataFiles(bool showSuccessMessage);

    // stores the current file paths so the next launch can reopen them
    void rememberDataFilePaths() const;

    // the widget tree built from the Designer layout, owned by this window
    Ui::ExpenseTrackerWindow* ui;
    // the in memory store every tab reads from and writes to
    TransactionManager transactionManager;
    // computes the totals and budget usage shown on the analytics tab
    AnalyticsEngine<> analyticsEngine;

    // empty while adding a new transaction, holds the target id while editing an existing one
    QString editingTransactionId;
    // tracks whether memory has moved ahead of the data files, so closing can warn about it
    bool hasUnsavedChanges = false;
};

#endif //EXPENSETRACKER_GUI_H