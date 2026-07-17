#include "include/GUI.h"
#include "ui_GUI.h"
#include <QHeaderView>
#include <QFileDialog>

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