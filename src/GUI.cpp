#include "include/GUI.h"
#include "ui_GUI.h"
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include "include/Expense.h"
#include "include/Income.h"

// formats a monetary value the way the analytics tab displays it
static QString formatCurrency(double value) {
    return QString("$%1").arg(value, 0, 'f', 2);
}

// turns a data manager error code into something worth showing the user
static QString dataErrorMessage(DataManager::DataReadWriteError error) {
    switch (error) {
        case DataManager::DataReadWriteError::NO_SUCH_TRANSACTION_FILE:
            return "The transactions file could not be found.";
        case DataManager::DataReadWriteError::NO_SUCH_CATEGORY_FILE:
            return "The categories file could not be found.";
        case DataManager::DataReadWriteError::CANNOT_OPEN_FILE:
            return "The file couldn't be opened.";
        default:
            return "An unknown error occurred while reading the data.";
    }
}

// builds the widget tree
GUI::GUI(QWidget* parent) : QWidget(parent), ui(new Ui::ExpenseTrackerWindow) {
    ui->setupUi(this);

    // dont allow future dated transactions
    ui->inputDate->setMaximumDate(QDate::currentDate());
    ui->inputDate->setDate(QDate::currentDate());

    // https://stackoverflow.com/questions/15686501/how-to-resize-qtablewidget-columns
    // stretch table columns evenly across the full table width
    ui->tableTransactions->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableCategories->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableBudgetUsage->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // https://forum.qt.io/topic/139637/how-to-create-a-button-and-place-it-where-i-want-and-then-run-a-function-when-the-button-is-clicked-c-qt/3
    // connect(myButton, &QPushButton::clicked, this, &MyClass::myFunction);
    connect(ui->btnAddTransaction, &QPushButton::clicked, this, &GUI::onAddTransactionClicked);
    connect(ui->btnCancelTransaction, &QPushButton::clicked, this, &GUI::onCancelTransactionClicked);
    connect(ui->comboType, &QComboBox::currentTextChanged, this, &GUI::onTypeChanged);
    connect(ui->btnBrowseTransactionsFile, &QPushButton::clicked, this, &GUI::onBrowseTransactionsClicked);
    connect(ui->btnBrowseCategoriesFile, &QPushButton::clicked, this, &GUI::onBrowseCategoriesClicked);
    connect(ui->btnRefreshAnalytics, &QPushButton::clicked, this, &GUI::onRefreshAnalyticsClicked);
    connect(ui->btnLoadData, &QPushButton::clicked, this, &GUI::onLoadDataClicked);
    connect(ui->btnCancelCategory, &QPushButton::clicked, this, &GUI::onCancelCategoryClicked);
    connect(ui->btnClearFilters, &QPushButton::clicked, this, &GUI::onClearFiltersClicked);
    // the date filter is opt in so we check for a toggled signal, then enable the date slot.
    connect(ui->chkFilterDate, &QCheckBox::toggled, ui->inputFilterDate, &QDateEdit::setEnabled);
    ui->inputFilterDate->setEnabled(false);
}

// free ui class mem
GUI::~GUI() {
    delete ui;
}

// jumps to the transaction form tab
void GUI::onAddTransactionClicked() const {
    ui->tabWidget->setCurrentWidget(ui->Tab_TransactionForm);
}

// returns to the main tab without saving, discarding whatever was typed
void GUI::onCancelTransactionClicked() const {
    clearTransactionForm();
    ui->tabWidget->setCurrentWidget(ui->Tab_Main);
}

// resets the transaction form back to its default state
void GUI::clearTransactionForm() const {
    ui->inputTransactionID->clear();
    ui->inputDate->setDate(QDate::currentDate());
    ui->inputAmount->clear();
    ui->inputCategory->clear();
    ui->inputDescription->clear();
    ui->inputExtra->clear();
}

// resets the category form back to its default state
void GUI::clearCategoryForm() const {
    ui->inputCategoryName->clear();
    ui->inputMonthlyBudget->clear();
}

// relabels the extra field to match the selected transaction type
void GUI::onTypeChanged(const QString& type) const {
    ui->labelExtra->setText(type == "Income" ? "Source:" : "Payment Method:");
}

// returns to the main tab without saving the category, discarding whatever was typed
void GUI::onCancelCategoryClicked() const {
    clearCategoryForm();
    ui->tabWidget->setCurrentWidget(ui->Tab_Main);
}

// clears the filters on the main tab
void GUI::onClearFiltersClicked() const {
    ui->chkFilterDate->setChecked(false);
    ui->inputFilterDate->setDate(QDate::currentDate());
    ui->inputFilterCategory->clear();
    ui->inputFilterAmount->clear();
}

void GUI::onBrowseTransactionsClicked() {
    QString path = QFileDialog::getOpenFileName(this, "Select Transactions File", QString(), "JSON files (*.json);;All Files (*)");
    if (!path.isEmpty()) {
        ui->inputTransactionsFilePath->setText(path);
    }
}

void GUI::onBrowseCategoriesClicked() {
        QString path = QFileDialog::getOpenFileName(this, "Select Categories File", QString(), "JSON files (*.json);;All Files (*)");
        if (!path.isEmpty()) {
            ui->inputCategoriesFilePath->setText(path);
        }
    }

// recalculates the analytics tab from the currently loaded data
void GUI::onRefreshAnalyticsClicked() {
    const auto searchResult = transactionManager.getAllTransactions();
    if (!searchResult.has_value()) {
        QMessageBox::warning(this, "Analytics", "Could not read the loaded transactions.");
        return;
    }

    const std::vector<const Transaction*>& transactions = searchResult.value();
    const std::vector<const Category*> categories = transactionManager.getAllCategories();

    ui->valueTotalIncome->setText(formatCurrency(analyticsEngine.computeTotalIncome(transactions)));
    ui->valueTotalExpenses->setText(formatCurrency(analyticsEngine.computeTotalExpenses(transactions)));
    const QString color = (analyticsEngine.computeSavings(transactions) < 0) ? "#FF0000" : "#008000";
    ui->valueSavings->setText("<font color='" + color + "'>" + formatCurrency(analyticsEngine.computeSavings(transactions)) + "</font>");
    const auto rows = analyticsEngine.computeBudgetUsage(transactions, categories);

    ui->tableBudgetUsage->setRowCount(static_cast<int>(rows.size()));
    for (int i = 0; i < static_cast<int>(rows.size()); i++) {
        const auto& row = rows.at(i);

        ui->tableBudgetUsage->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(row.categoryName)));
        ui->tableBudgetUsage->setItem(i, 1, new QTableWidgetItem(formatCurrency(row.budget)));
        ui->tableBudgetUsage->setItem(i, 2, new QTableWidgetItem(formatCurrency(row.spent)));
        ui->tableBudgetUsage->setItem(i, 3, new QTableWidgetItem(QString::number(row.usagePercent, 'f', 1) + "%"));
    }
}

// fills the main tab table from everything currently loaded. private helper so theres no slot that ->
// needs to be allocated for it since its called through onLoadDataClicked
void GUI::refreshTransactionsTable() {
    const auto searchResult = transactionManager.getAllTransactions();
    if (!searchResult.has_value()) {
        QMessageBox::warning(this, "Transactions", "Could not read the loaded transactions.");
        return;
    }
    const std::vector<const Transaction*>& transactions = searchResult.value();

    ui->tableTransactions->setRowCount(static_cast<int>(transactions.size())); // allocate the correct amount of rows based on our transactions
    for (int i = 0; i < static_cast<int>(transactions.size()); i++) {
        const Transaction* t = transactions.at(i);

        QString type;
        QString extra; // wheere extra is just the payment method
        if (const auto* expense = dynamic_cast<const Expense*>(t)) { //downcast, find out if transaction* is actually pointing at an expense
            type = "Expense";
            extra = QString::fromStdString(expense->getPaymentMethod());
        } else if (const auto* income = dynamic_cast<const Income*>(t)) {
            type = "Income";
            extra = QString::fromStdString(income->getSource());
        }

        // populate data in each visible column in the main tab
        ui->tableTransactions->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(t->getTransactionID())));
        ui->tableTransactions->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(t->getDate())));
        ui->tableTransactions->setItem(i, 2, new QTableWidgetItem(formatCurrency(t->getAmount())));
        ui->tableTransactions->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(t->getCategory())));
        ui->tableTransactions->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(t->getDescription())));
        ui->tableTransactions->setItem(i, 5, new QTableWidgetItem(type));
        ui->tableTransactions->setItem(i, 6, new QTableWidgetItem(extra));
    }
}


// reads both data files listed on the main tab into the transaction manager
void GUI::onLoadDataClicked() {
    const QString transactionsPath = ui->inputTransactionsFilePath->text(); // load transaction file from this location
    const QString categoriesPath = ui->inputCategoriesFilePath->text(); // load category file from this location

    if (transactionsPath.isEmpty() || categoriesPath.isEmpty()) {
        QMessageBox::warning(this, "Load Data",
            "Select both a transactions file and a categories file first.");
        return;
    }

    const DataManager dataManager(transactionsPath.toStdString(), categoriesPath.toStdString());
    try {
        const auto result = transactionManager.load(dataManager);
        if (!result.has_value()) {
            QMessageBox::warning(this, "Load Data", dataErrorMessage(result.error()));
            return;
        }

        QMessageBox::information(this, "Load Data", QString("Loaded %1 records.").arg(result.value())); // total num of transactions & categories
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Load Data", QString("Failed to read the data files:\n%1").arg(e.what()));
        return;
    }


    refreshTransactionsTable();
    onRefreshAnalyticsClicked();
}