#include "../include/GUI.h"

#include "ui_GUI.h"

// builds the widget tree
GUI::GUI(QWidget* parent) : QWidget(parent), ui(new Ui::ExpenseTrackerWindow) {
    ui->setupUi(this);
}

// free ui class mem
GUI::~GUI() {
    delete ui;
}
