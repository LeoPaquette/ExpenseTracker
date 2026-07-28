#include "include/GUI.h"
#include "ui_GUI.h"
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>

// formats a monetary value the way the analytics tab displays it
static QString formatCurrency(double value) {
    return QString("$%1").arg(value, 0, 'f', 2);
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
    // TEMPORARY: seed categories so the analytics tab has something to show
    transactionManager.addCategory(Category("CAT-0001", "Food", "500"));
    transactionManager.addCategory(Category("CAT-0002", "Rent", "1500"));
}

// free ui class mem
GUI::~GUI() {
    delete ui;
}

// jumps to the transaction form tab
void GUI::onAddTransactionClicked() {
    ui->tabWidget->setCurrentWidget(ui->Tab_TransactionForm);
}

// returns to the main tab without saving
void GUI::onCancelTransactionClicked() {
    ui->tabWidget->setCurrentWidget(ui->Tab_Main);
}

// relabels the extra field to match the selected transaction type
void GUI::onTypeChanged(const QString& type) {
    ui->labelExtra->setText(type == "Income" ? "Source:" : "Payment Method:");
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
    const auto searchResult = transactionManager.searchTransactions(std::nullopt, std::nullopt, std::nullopt);
    if (!searchResult.has_value()) {
        QMessageBox::warning(this, "Analytics", "Could not read the loaded transactions.");
        return;
    }

    const std::vector<const Transaction*>& transactions = searchResult.value();
    const std::vector<const Category*> categories = transactionManager.getAllCategories();

    ui->valueTotalIncome->setText(formatCurrency(analyticsEngine.computeTotalIncome(transactions)));
    ui->valueTotalExpenses->setText(formatCurrency(analyticsEngine.computeTotalExpenses(transactions)));
    ui->valueSavings->setText(formatCurrency(analyticsEngine.computeSavings(transactions)));

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