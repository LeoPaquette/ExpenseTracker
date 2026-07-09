#include "../include/GUI.h"

#include "ui_GUI.h"

// builds the widget tree
GUI::GUI(QWidget* parent) : QWidget(parent), ui(new Ui::ExpenseTrackerWindow) {
    ui->setupUi(this);

    // dont allow future dated transactions
    ui->inputDate->setMaximumDate(QDate::currentDate());
    ui->inputDate->setDate(QDate::currentDate());

    // https://forum.qt.io/topic/139637/how-to-create-a-button-and-place-it-where-i-want-and-then-run-a-function-when-the-button-is-clicked-c-qt/3
    // connect(myButton, &QPushButton::clicked, this, &MyClass::myFunction);
    // connect the button, signal, context and slot
    connect(ui->btnAddTransaction, &QPushButton::clicked, this, &GUI::onAddTransactionClicked);
    connect(ui->btnCancelTransaction, &QPushButton::clicked, this, &GUI::onCancelTransactionClicked);
    connect(ui->comboType, &QComboBox::currentTextChanged, this, &GUI::onTypeChanged);
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