/*
 * CST8219 - Personal Expense Tracker and Budget Management System
 * This files handles all GUI logic.
 * @author Wesley Deir
*/
#include "include/GUI.h"
#include "ui_GUI.h"
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QProgressBar>
#include <algorithm>
#include "include/Expense.h"
#include "include/Income.h"

// the two budget thresholds the spec asks us to warn on
static constexpr double BUDGET_WARNING_PERCENT = 80.0;
static constexpr double BUDGET_OVER_PERCENT = 100.0;

// where QSettings keeps the file paths we reopen on the next launch
static const QString SETTINGS_ORGANIZATION = "CST8219";
static const QString SETTINGS_APPLICATION = "ExpenseTracker";
static const QString SETTINGS_KEY_TRANSACTIONS = "transactionsFile";
static const QString SETTINGS_KEY_CATEGORIES = "categoriesFile";

// the theme colours the code has to know about. item foregrounds and cell widgets are built in
// c++, and neither can be reached from the stylesheet, so these have to match dark.qss by hand
static const QString COLOR_POSITIVE = "#34d399";
static const QString COLOR_WARNING = "#fbbf24";
static const QString COLOR_OVER = "#f87171";

// deeper versions of the same three, for the budget bars. the percentage is printed over the top
// of the fill, and the bright shades above are too light to read pale text against
static const QString FILL_POSITIVE = "#15803d";
static const QString FILL_WARNING = "#a16207";
static const QString FILL_OVER = "#b91c1c";

/**
 * @brief Builds a table cell for a money value.
 *
 * Right aligned and monospaced, both of which are needed for the decimal points to line up down
 * a column. Proportional digits are different widths, so right alignment on its own still leaves
 * $1200.00 and $112.45 ragged. A column of figures you can compare at a glance is most of what
 * separates a table of money from a table of text.
 *
 * @param text The already formatted value, e.g. @c "$120.50".
 * @return A cell ready to hand to @c setItem.
 */
static QTableWidgetItem* moneyItem(const QString& text) {
    // built once. the size follows whatever the application font is using so it sits on the same
    // baseline as the rest of the row
    static const QFont figures = [] {
        QFont font("Consolas");
        font.setStyleHint(QFont::Monospace);
        font.setPointSize(QApplication::font().pointSize());
        return font;
    }();

    auto* item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    item->setFont(figures);
    return item;
}

/**
 * @brief Puts the application into the dark theme.
 *
 * Three things have to happen together. Fusion replaces the native windows style, which paints
 * half of these widgets itself and ignores most of a stylesheet. The palette covers everything
 * the stylesheet doesnt name, so scroll bars and the calendar popup dont open in light grey.
 * The stylesheet itself then does the borders, the rounding and the hover states.
 *
 * Applied to the whole application rather than just this window so message boxes and tooltips,
 * which arent part of our widget tree, get themed too.
 *
 * https://doc.qt.io/qt-6/qpalette.html
 *   the colour roles below. Window is the widget background, Base is the background of anything
 *   you type into, Highlight is the selection colour. read it to see why setting Text alone
 *   isnt enough.
 * https://doc.qt.io/qt-6/qstylefactory.html
 *   where "Fusion" comes from. qt picks the native windows style by default, and that one draws
 *   a lot of widgets itself and quietly ignores most of a stylesheet.
 * https://doc.qt.io/qt-6/resources.html
 *   the ":/styles/dark.qss" path and the AUTORCC line in CMakeLists.txt. the prefix is set in
 *   resources/resources.qrc.
 */
static void applyTheme() {
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QPalette dark;
    dark.setColor(QPalette::Window, QColor("#111418"));
    dark.setColor(QPalette::WindowText, QColor("#e6e9ec"));
    dark.setColor(QPalette::Base, QColor("#181c21"));
    dark.setColor(QPalette::AlternateBase, QColor("#1c2126"));
    dark.setColor(QPalette::Text, QColor("#e6e9ec"));
    dark.setColor(QPalette::Button, QColor("#20252b"));
    dark.setColor(QPalette::ButtonText, QColor("#e6e9ec"));
    dark.setColor(QPalette::Highlight, QColor("#2dd4a7"));
    dark.setColor(QPalette::HighlightedText, QColor("#0d1013"));
    dark.setColor(QPalette::ToolTipBase, QColor("#20252b"));
    dark.setColor(QPalette::ToolTipText, QColor("#e6e9ec"));
    dark.setColor(QPalette::PlaceholderText, QColor("#7d858f"));
    dark.setColor(QPalette::Disabled, QPalette::Text, QColor("#545b64"));
    dark.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#545b64"));
    QApplication::setPalette(dark);

    // compiled in by AUTORCC, so theres no file next to the exe that can go missing
    QFile styleFile(":/styles/dark.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        qApp->setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }
    // if it somehow didnt open we still have the palette, so the app is dark either way
}

// formats a monetary value the way the analytics tab displays it
static QString formatCurrency(double value) {
    return QString("$%1").arg(value, 0, 'f', 2);
}

// spells out the budget warning next to the percentage so it reads as a warning, not just a colour
static QString budgetUsageText(double usagePercent) {
    QString percent = QString::number(usagePercent, 'f', 1) + "%";

    if (usagePercent > BUDGET_OVER_PERCENT) {
        return percent + "  OVER BUDGET";
    }
    if (usagePercent > BUDGET_WARNING_PERCENT) {
        return percent + "  WARNING";
    }
    return percent;
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
        case DataManager::DataReadWriteError::DUPLICATE_RECORDS:
            return "The data files contain duplicate IDs, so nothing was loaded.";
        default:
            return "An unknown error occurred while reading the data.";
    }
}

// turns a search error code into something worth showing the user
static QString searchErrorMessage(TransactionManager::SearchError error) {
    switch (error) {
        case TransactionManager::SearchError::INVALID_DATE_FORMAT:
            return "The date filter isn't in YYYY-MM-DD format.";
        case TransactionManager::SearchError::INVALID_DATE_FUTURE:
            return "The date filter can't be in the future.";
        case TransactionManager::SearchError::INVALID_AMOUNT_EXCEEDS_MAX:
            return "The amount filter is above the $1,000,000 maximum.";
        case TransactionManager::SearchError::INVALID_AMOUNT_NEGATIVE:
            return "The amount filter can't be negative.";
        default:
            return "An unknown error occurred while searching.";
    }
}

// builds the widget tree
GUI::GUI(QWidget* parent) : QWidget(parent), ui(new Ui::ExpenseTrackerWindow) {
    applyTheme();
    ui->setupUi(this);

    // the tables read much better banded once theres a surface colour behind them, and once
    // theyre banded the grid lines are a second way of saying the same thing. one is enough
    for (QTableWidget* table : {ui->tableTransactions, ui->tableCategories, ui->tableBudgetUsage}) {
        table->setAlternatingRowColors(true);
        table->setShowGrid(false);
    }

    // the money cells are right aligned, so a centred heading over them sits off to one side of
    // its own column. these are the columns moneyItem() fills
    ui->tableTransactions->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableCategories->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableBudgetUsage->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableBudgetUsage->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // dont allow future dated transactions
    ui->inputDate->setMaximumDate(QDate::currentDate());
    ui->inputDate->setDate(QDate::currentDate());

    // select the whole row rather than one cell so Edit/Delete have an obvious target
    ui->tableTransactions->setSelectionBehavior(QAbstractItemView::SelectRows);
    // the tables only display data, typing into a cell wouldnt reach the transaction manager
    ui->tableTransactions->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableCategories->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableBudgetUsage->setEditTriggers(QAbstractItemView::NoEditTriggers);

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
    connect(ui->btnSaveData, &QPushButton::clicked, this, &GUI::onSaveDataClicked);
    connect(ui->btnCancelCategory, &QPushButton::clicked, this, &GUI::onCancelCategoryClicked);
    connect(ui->btnClearFilters, &QPushButton::clicked, this, &GUI::onClearFiltersClicked);
    connect(ui->btnSearchTransactions, &QPushButton::clicked, this, &GUI::onSearchTransactionsClicked);
    connect(ui->btnDeleteTransaction, &QPushButton::clicked, this, &GUI::onDeleteTransactionClicked);
    connect(ui->btnSaveTransaction, &QPushButton::clicked, this, &GUI::onSaveTransactionClicked);
    connect(ui->btnSaveCategory, &QPushButton::clicked, this, &GUI::onSaveCategoryClicked);
    connect(ui->btnEditTransaction, &QPushButton::clicked, this, &GUI::onEditTransactionClicked);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &GUI::onTabChanged);
    // double clicking a row pops up the detail view. cellDoubleClicked also hands over the column,
    // which we dont care about, and qt lets a slot drop trailing arguments it doesnt want
    connect(ui->tableTransactions, &QTableWidget::cellDoubleClicked, this, &GUI::onTransactionRowDoubleClicked);
    connect(ui->tableCategories, &QTableWidget::cellDoubleClicked, this, &GUI::onCategoryRowDoubleClicked);
    // the date filter is opt in so we check for a toggled signal, then enable the date slot.
    connect(ui->chkFilterDate, &QCheckBox::toggled, ui->inputFilterDate, &QDateEdit::setEnabled);
    ui->inputFilterDate->setEnabled(false);
    ui->inputFilterDate->setDate(QDate::currentDate());

    // the spec wants previous data back on startup. rather than hardcoding the data/ folder we
    // reopen whichever files were last used, which QSettings remembered for us on the way out
    const QSettings settings(SETTINGS_ORGANIZATION, SETTINGS_APPLICATION);
    ui->inputTransactionsFilePath->setText(settings.value(SETTINGS_KEY_TRANSACTIONS).toString());
    ui->inputCategoriesFilePath->setText(settings.value(SETTINGS_KEY_CATEGORIES).toString());

    if (!ui->inputTransactionsFilePath->text().isEmpty() && !ui->inputCategoriesFilePath->text().isEmpty()) {
        loadDataFiles(false); // quietly, nobody wants a popup just for opening the app
    }
}

// free ui class mem
GUI::~GUI() {
    delete ui;
}

// jumps to the transaction form tab with a blank form, so Add never inherits an edit in progress
void GUI::onAddTransactionClicked() {
    clearTransactionForm();
    ui->tabWidget->setCurrentWidget(ui->Tab_TransactionForm);
}

// returns to the main tab without saving, discarding whatever was typed
void GUI::onCancelTransactionClicked() {
    clearTransactionForm();
    ui->tabWidget->setCurrentWidget(ui->Tab_Main);
}

// resets the transaction form back to a blank add-mode state
void GUI::clearTransactionForm() {
    ui->inputTransactionID->clear();
    ui->inputDate->setDate(QDate::currentDate());
    ui->inputAmount->clear();
    ui->inputCategory->clear();
    ui->inputDescription->clear();
    ui->inputExtra->clear();

    // back to adding, so the id and the type become the user's to pick again
    editingTransactionId.clear();
    ui->inputTransactionID->setReadOnly(false);
    ui->comboType->setEnabled(true);
}

// resets the category form back to its default state
void GUI::clearCategoryForm() const {
    ui->inputCategoryName->clear();
    ui->inputMonthlyBudget->clear();
}

// recalculates the analytics tab whenever the user switches to it, so the totals cant sit there
// stale after an add or a delete on another tab
void GUI::onTabChanged(const int index) {
    if (ui->tabWidget->widget(index) == ui->tab_Analytics) {
        onRefreshAnalyticsClicked();
    }
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

// loads whichever transaction is selected in the main tab table into the form for editing
void GUI::onEditTransactionClicked() {
    const int row = ui->tableTransactions->currentRow(); // -1 when nothing is selected
    if (row < 0) {
        QMessageBox::information(this, "Edit Transaction", "Select a transaction in the table first.");
        return;
    }

    // every field the form needs is already sitting in the row, so theres nothing to look up.
    // setting the type first lets the existing onTypeChanged slot relabel the extra field for us
    ui->comboType->setCurrentText(ui->tableTransactions->item(row, 5)->text());
    ui->inputTransactionID->setText(ui->tableTransactions->item(row, 0)->text());
    ui->inputDate->setDate(QDate::fromString(ui->tableTransactions->item(row, 1)->text(), "yyyy-MM-dd"));
    ui->inputAmount->setText(
        QString::number(ui->tableTransactions->item(row, 2)->data(Qt::UserRole).toDouble(), 'f', 2));
    ui->inputCategory->setText(ui->tableTransactions->item(row, 3)->text());
    ui->inputDescription->setText(ui->tableTransactions->item(row, 4)->text());
    ui->inputExtra->setText(ui->tableTransactions->item(row, 6)->text());

    // editTransaction finds its target by id, and throws if the type changed under it, so we lock both
    editingTransactionId = ui->tableTransactions->item(row, 0)->text();
    ui->inputTransactionID->setReadOnly(true);
    ui->comboType->setEnabled(false);

    ui->tabWidget->setCurrentWidget(ui->Tab_TransactionForm);
}

// builds a category from the category form and adds it
void GUI::onSaveCategoryClicked() {
    // toDouble reports failure through ok, so a non number gets its own message instead of
    // reaching the constructor as a perfectly valid looking 0
    bool ok = false;
    const double monthlyBudget = ui->inputMonthlyBudget->text().trimmed().toDouble(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Save Category", "The monthly budget has to be a number.");
        return;
    }

    const QString name = ui->inputCategoryName->text().trimmed();

    try {
        // the manager hands out the number, we only pad it into the CAT-0000 shape its regex wants
        const QString categoryID =
            QString("CAT-%1").arg(transactionManager.getNextCategoryId(), 4, 10, QChar('0'));

        // the Category constructor owns every field rule and throws naming whichever one failed
        const Category category(categoryID.toStdString(), name.toStdString(), monthlyBudget);

        // false means the id or the name was already taken, which is the category half of Feature 2
        if (!transactionManager.addCategory(category)) {
            QMessageBox::warning(this, "Save Category",
                QString("A category named \"%1\" already exists.").arg(name));
            return;
        }
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Save Category", e.what());
        return;
    }

    hasUnsavedChanges = true;

    // stay on this tab so the new row appears in the table right below the form
    clearCategoryForm();
    refreshCategoriesTable();
    onRefreshAnalyticsClicked();
}

// builds a transaction from the form and either adds it or updates the one being edited
void GUI::onSaveTransactionClicked() {
    // toDouble reports failure through ok rather than throwing, and the constructor cant catch this
    // for us because a non number comes through as a perfectly valid looking 0
    bool ok = false;
    const double amount = ui->inputAmount->text().trimmed().toDouble(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Save Transaction", "The amount has to be a number.");
        return;
    }

    const QString transactionID = ui->inputTransactionID->text().trimmed();
    const bool isExpense = ui->comboType->currentText() == "Expense";

    // one unusually large expense is worth a second look before it lands in the table. the threshold
    // is the users to set, and income is exempt because a big paycheque isnt what this is here to catch
    const double spendingThreshold = ui->inputSpendingThreshold->value();
    if (isExpense && amount > spendingThreshold) {
        const auto confirmed = QMessageBox::question(this, "Spending Alert",
            QString("This expense exceeds %1.\nAre you sure you want to continue?")
                .arg(formatCurrency(spendingThreshold)));
        if (confirmed != QMessageBox::Yes) {
            return;
        }
    }

    // whether we are adding or updating only changes which manager call the new object goes to.
    // addTransaction returns false when the id is already taken, editTransaction is meant to reuse one
    const auto store = [this](const auto& transaction) {
        if (editingTransactionId.isEmpty()) {
            return transactionManager.addTransaction(transaction);
        }

        transactionManager.editTransaction(transaction);
        return true;
    };

    // every field rule already lives in the constructors, so building the object IS the validation.
    // an invalid field throws with a message naming that field, which is what we show the user
    bool stored = false;
    try {
        const std::string date = ui->inputDate->date().toString("yyyy-MM-dd").toStdString();
        const std::string category = ui->inputCategory->text().toStdString();
        const std::string description = ui->inputDescription->text().toStdString();
        const std::string methodOrSource = ui->inputExtra->text().toStdString(); // whichever the type wants

        if (isExpense) {
            stored = store(Expense(transactionID.toStdString(), date, amount, category, description, methodOrSource));
        } else {
            stored = store(Income(transactionID.toStdString(), date, amount, category, description, methodOrSource));
        }
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Save Transaction", e.what());
        return;
    }

    // the manager rejected it, which for an add can only mean the id is already in use
    if (!stored) {
        QMessageBox::warning(this, "Save Transaction",
            QString("Transaction %1 already exists.").arg(transactionID));
        return;
    }

    hasUnsavedChanges = true;

    // clearing the form empties the id, so a second rapid click is rejected by the constructor
    clearTransactionForm();
    refreshTransactionsTable();
    onRefreshAnalyticsClicked();
    ui->tabWidget->setCurrentWidget(ui->Tab_Main);
}

// clears the filters on the main tab and puts every transaction back in the table
void GUI::onClearFiltersClicked() {
    ui->chkFilterDate->setChecked(false);
    ui->inputFilterDate->setDate(QDate::currentDate());
    ui->inputFilterCategory->clear();
    ui->inputFilterAmount->clear();

    refreshTransactionsTable();
}

// deletes whichever transaction is selected in the main tab table
void GUI::onDeleteTransactionClicked() {
    const int row = ui->tableTransactions->currentRow(); // -1 when nothing is selected
    if (row < 0) {
        QMessageBox::information(this, "Delete Transaction", "Select a transaction in the table first.");
        return;
    }

    const QString transactionID = ui->tableTransactions->item(row, 0)->text(); // column 0 holds the id

    // deleting cant be undone, so make the user confirm before we touch the manager
    const auto confirmed = QMessageBox::question(this, "Delete Transaction",
        QString("Delete transaction %1?").arg(transactionID));
    if (confirmed != QMessageBox::Yes) {
        return;
    }

    transactionManager.deleteTransaction(transactionID.toStdString());
    hasUnsavedChanges = true;

    refreshTransactionsTable();
    onRefreshAnalyticsClicked();
}

// shows the full details of whichever transaction row was double clicked
void GUI::onTransactionRowDoubleClicked(int row) {
    const auto searchResult = transactionManager.getAllTransactions();
    if (!searchResult.has_value()) {
        QMessageBox::warning(this, "Transaction Details", "Could not read the loaded transactions.");
        return;
    }

    const QString transactionID = ui->tableTransactions->item(row, 0)->text(); // column 0 holds the id

    // the row only carries display text, and displayTransaction lives on the transaction itself. its
    // virtual, so finding the stored object is what gets us the Expense or the Income wording rather
    // than the base one. no else branch because the table was built from this same vector
    for (const Transaction* t : searchResult.value()) {
        if (QString::fromStdString(t->getTransactionID()) == transactionID) {
            QMessageBox::information(this, "Transaction Details",
                QString::fromStdString(t->displayTransaction()));
            return;
        }
    }
}

// shows the budget summary of whichever category row was double clicked
void GUI::onCategoryRowDoubleClicked(int row) {
    const auto searchResult = transactionManager.getAllTransactions();
    if (!searchResult.has_value()) {
        QMessageBox::warning(this, "Category Details", "Could not read the loaded transactions.");
        return;
    }

    // displayCategorySummary wants the amount spent, which only the analytics engine works out
    const auto spending = analyticsEngine.computeCategorySpending(searchResult.value());

    const QString categoryName = ui->tableCategories->item(row, 0)->text(); // column 0 holds the name

    for (const Category* c : transactionManager.getAllCategories()) {
        if (QString::fromStdString(c->getName()) == categoryName) {
            // the map only has the categories something was actually spent against, so a category
            // nobody has spent on is missing from it rather than sitting in there as a zero
            const auto spent = spending.find(c->getName());
            const double amountSpent = (spent != spending.end()) ? spent->second : 0.0;

            QMessageBox::information(this, "Category Details",
                QString::fromStdString(c->displayCategorySummary(amountSpent)));
            return;
        }
    }
}

// filters the main tab table down to the transactions matching whichever filters are filled in
void GUI::onSearchTransactionsClicked() {
    // a filter left blank stays as nullopt, which searchTransactions treats as "dont filter on this"
    const std::string date = ui->inputFilterDate->date().toString("yyyy-MM-dd").toStdString();
    const std::optional<const std::string*> dateFilter =
        ui->chkFilterDate->isChecked() ? std::optional(&date) : std::nullopt;

    std::optional<const Category*> categoryFilter = std::nullopt;
    if (const QString categoryName = ui->inputFilterCategory->text().trimmed(); !categoryName.isEmpty()) {
        // searchTransactions wants a real Category, and we cant invent one because the Category
        // constructor demands a valid CAT-0000 id and a positive budget. so we look the name up
        const std::vector<const Category*> categories = transactionManager.getAllCategories();
        for (const Category* c : categories) {
            if (QString::fromStdString(c->getName()).compare(categoryName, Qt::CaseInsensitive) == 0) {
                categoryFilter = c;
                break;
            }
        }

        if (!categoryFilter.has_value()) {
            QMessageBox::warning(this, "Search", QString("There's no category named \"%1\".").arg(categoryName));
            return;
        }
    }

    std::optional<double> amountFilter = std::nullopt;
    if (const QString amountText = ui->inputFilterAmount->text().trimmed(); !amountText.isEmpty()) {
        // toDouble reports failure through ok rather than throwing, and searchTransactions cant
        // catch this for us because a non number comes through as a perfectly valid looking 0
        bool ok = false;
        const double amount = amountText.toDouble(&ok);
        if (!ok) {
            QMessageBox::warning(this, "Search", "The amount filter has to be a number.");
            return;
        }
        amountFilter = amount;
    }

    const auto searchResult = transactionManager.searchTransactions(dateFilter, categoryFilter, amountFilter);
    if (!searchResult.has_value()) {
        QMessageBox::warning(this, "Search", searchErrorMessage(searchResult.error()));
        return;
    }

    populateTransactionsTable(searchResult.value());
}

// picks the transactions file through a file dialog. an empty path means the user cancelled,
// in which case we leave whatever was already selected alone
void GUI::onBrowseTransactionsClicked() {
    QString path = QFileDialog::getOpenFileName(this, "Select Transactions File", QString(), "JSON files (*.json);;All Files (*)");
    if (!path.isEmpty()) {
        ui->inputTransactionsFilePath->setText(path);
    }
}

// picks the categories file the same way the transactions one is picked
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
    const QString color = (analyticsEngine.computeSavings(transactions) < 0) ? COLOR_OVER : COLOR_POSITIVE;
    ui->valueSavings->setText("<font color='" + color + "'>" + formatCurrency(analyticsEngine.computeSavings(transactions)) + "</font>");
    const auto rows = analyticsEngine.computeBudgetUsage(transactions, categories);

    ui->tableBudgetUsage->setRowCount(static_cast<int>(rows.size()));
    for (int i = 0; i < static_cast<int>(rows.size()); i++) {
        const auto& row = rows.at(i);

        ui->tableBudgetUsage->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(row.categoryName)));
        ui->tableBudgetUsage->setItem(i, 1, moneyItem(formatCurrency(row.budget)));
        ui->tableBudgetUsage->setItem(i, 2, moneyItem(formatCurrency(row.spent)));

        // whichever of the three bands the row is in. the bright shade labels the category, the
        // deep one fills the bar behind the percentage
        const bool isOver = row.usagePercent > BUDGET_OVER_PERCENT;
        const QString usageColor = isOver ? COLOR_OVER
                                 : (row.usagePercent > BUDGET_WARNING_PERCENT) ? COLOR_WARNING
                                 : COLOR_POSITIVE;
        const QString usageFill = isOver ? FILL_OVER
                                : (row.usagePercent > BUDGET_WARNING_PERCENT) ? FILL_WARNING
                                : FILL_POSITIVE;

        // a bar rather than a number. the figure tells you what the usage is, the bar tells you how
        // close to the edge it is without having to read anything, which is the actual question
        auto* usageBar = new QProgressBar();
        usageBar->setRange(0, 100);
        usageBar->setValue(static_cast<int>(std::min(row.usagePercent, 100.0))); // 125% still fills it, no more
        usageBar->setFormat(budgetUsageText(row.usagePercent)); // the exact figure stays, printed on the bar
        usageBar->setAlignment(Qt::AlignCenter);
        // everything else about the bar is in the stylesheet. only the fill changes per row
        usageBar->setStyleSheet("QProgressBar::chunk { background-color: " + usageFill + "; border-radius: 4px; }");
        ui->tableBudgetUsage->setCellWidget(i, 3, usageBar);

        // the bar already carries the warning, so the text says it once more and quietly. colouring
        // all four columns on top of a coloured bar is the same alarm going off twice
        if (row.usagePercent > BUDGET_WARNING_PERCENT) {
            ui->tableBudgetUsage->item(i, 0)->setForeground(QColor(usageColor));
        }
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

    populateTransactionsTable(searchResult.value());
}

// fills the main tab table with whichever transactions it gets handed, so a full refresh and a
// filtered search can share the same row building code
void GUI::populateTransactionsTable(const std::vector<const Transaction*>& transactions) const {
    ui->tableTransactions->setRowCount(static_cast<int>(transactions.size())); // allocate the correct amount of rows based on our transactions
    for (int i = 0; i < static_cast<int>(transactions.size()); i++) {
        const Transaction* t = transactions.at(i);

        QString type;
        QString methodOrSource; // one column for two different fields, whichever the type carries
        if (const auto* expense = dynamic_cast<const Expense*>(t)) { //downcast, find out if transaction* is actually pointing at an expense
            type = "Expense";
            methodOrSource = QString::fromStdString(expense->getPaymentMethod());
        } else if (const auto* income = dynamic_cast<const Income*>(t)) {
            type = "Income";
            methodOrSource = QString::fromStdString(income->getSource());
        }

        // income is the exception in a table thats almost entirely expenses, so its the only thing
        // that gets a sign and a colour. marking every expense red instead would turn the whole
        // table into a warning and stop any of it meaning anything
        const bool isIncome = (type == "Income");
        // the amount is the one column whose display text isnt its value, so we keep the raw number
        // on the item as well. that way Edit can read it straight back instead of unpicking "$120.50"
        QTableWidgetItem* amountItem = moneyItem((isIncome ? "+" : "") + formatCurrency(t->getAmount()));
        amountItem->setData(Qt::UserRole, t->getAmount());
        if (isIncome) {
            amountItem->setForeground(QColor(COLOR_POSITIVE));
        }

        // populate data in each visible column in the main tab
        ui->tableTransactions->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(t->getTransactionID())));
        ui->tableTransactions->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(t->getDate())));
        ui->tableTransactions->setItem(i, 2, amountItem);
        ui->tableTransactions->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(t->getCategory())));
        ui->tableTransactions->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(t->getDescription())));
        ui->tableTransactions->setItem(i, 5, new QTableWidgetItem(type));
        ui->tableTransactions->setItem(i, 6, new QTableWidgetItem(methodOrSource));
    }
}


// fills the category tab table from everything currently loaded
void GUI::refreshCategoriesTable() {
    const std::vector<const Category*> categories = transactionManager.getAllCategories();

    ui->tableCategories->setRowCount(static_cast<int>(categories.size())); // allocate the correct amount of rows based on our categories
    for (int i = 0; i < static_cast<int>(categories.size()); i++) {
        const Category* c = categories.at(i);

        ui->tableCategories->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(c->getName())));
        ui->tableCategories->setItem(i, 1, moneyItem(formatCurrency(c->getMonthlyBudget())));
    }
}


// stores the current file paths so the next launch can reopen them
void GUI::rememberDataFilePaths() const {
    QSettings settings(SETTINGS_ORGANIZATION, SETTINGS_APPLICATION);
    settings.setValue(SETTINGS_KEY_TRANSACTIONS, ui->inputTransactionsFilePath->text());
    settings.setValue(SETTINGS_KEY_CATEGORIES, ui->inputCategoriesFilePath->text());
}

// reads both data files listed on the main tab into the transaction manager
void GUI::onLoadDataClicked() {
    loadDataFiles(true); // the user asked for this one, so tell them how it went
}

// reads both data files listed on the main tab, optionally announcing how many records arrived.
// startup calls this quietly, the Load Data button calls it with the message
void GUI::loadDataFiles(const bool showSuccessMessage) {
    const QString transactionsPath = ui->inputTransactionsFilePath->text(); // load transaction file from this location
    const QString categoriesPath = ui->inputCategoriesFilePath->text(); // load category file from this location

    if (transactionsPath.isEmpty() || categoriesPath.isEmpty()) {
        QMessageBox::warning(this, "Load Data",
            "Select both a transactions file and a categories file first.");
        return;
    }

    const DataManager dataManager(transactionsPath.toStdString(), categoriesPath.toStdString());
    bool loaded = false;
    try {
        if (const auto result = transactionManager.load(dataManager); result.has_value()) {
            loaded = true;

            if (showSuccessMessage) {
                QMessageBox::information(this, "Load Data", QString("Loaded %1 records.").arg(result.value())); // total num of transactions & categories
            }
        } else {
            QMessageBox::warning(this, "Load Data", dataErrorMessage(result.error()));
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Load Data", QString("Failed to read the data files:\n%1").arg(e.what()));
    }

    // load() puts the previous data back when it returns an error, but a malformed file throws out of
    // it instead, and that path leaves the manager holding whatever was parsed before the throw. so we
    // rebuild from whatever it actually holds either way, rather than trusting the tables to be current
    transactionManager.refreshUsedCategoryIds();
    refreshTransactionsTable();
    refreshCategoriesTable();
    onRefreshAnalyticsClicked();

    if (!loaded) {
        return;
    }

    // only a clean load means memory matches the files, so only then is there nothing to warn about
    hasUnsavedChanges = false;
    rememberDataFilePaths();
}

// writes everything currently in memory out to both data files listed on the main tab
void GUI::onSaveDataClicked() {
    const QString transactionsPath = ui->inputTransactionsFilePath->text(); // write transactions to this location
    const QString categoriesPath = ui->inputCategoriesFilePath->text(); // write categories to this location

    if (transactionsPath.isEmpty() || categoriesPath.isEmpty()) {
        QMessageBox::warning(this, "Save Data",
            "Select both a transactions file and a categories file first.");
        return;
    }

    const DataManager dataManager(transactionsPath.toStdString(), categoriesPath.toStdString());
    try {
        const auto result = transactionManager.save(dataManager);
        if (!result.has_value()) {
            // a read only or locked file lands here as CANNOT_OPEN_FILE
            QMessageBox::warning(this, "Save Data", dataErrorMessage(result.error()));
            return;
        }

        QMessageBox::information(this, "Save Data", QString("Saved %1 records.").arg(result.value())); // total num of transactions & categories
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Save Data", QString("Failed to write the data files:\n%1").arg(e.what()));
        return;
    }

    // everything in memory is on disk now, so closing the window is safe again
    hasUnsavedChanges = false;
    rememberDataFilePaths();
}

// asks the user what to do about unsaved changes before the window actually closes
void GUI::closeEvent(QCloseEvent* event) {
    if (!hasUnsavedChanges) {
        event->accept();
        return;
    }

    const auto choice = QMessageBox::question(this, "Unsaved Changes",
        "You have unsaved changes.\nDo you want to save before exiting?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (choice == QMessageBox::Cancel) {
        event->ignore();
        return;
    }

    if (choice == QMessageBox::Save) {
        onSaveDataClicked();

        // saving still fails on a read only or locked file, and onSaveDataClicked only clears the
        // flag once the write actually worked. so if its still set, stay open rather than lose the data
        if (hasUnsavedChanges) {
            event->ignore();
            return;
        }
    }

    event->accept();
}