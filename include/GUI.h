#ifndef EXPENSETRACKER_GUI_H
#define EXPENSETRACKER_GUI_H

#include <QWidget>

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
    void onAddTransactionClicked();
    // returns to the main tab WITHOUT saving
    void onCancelTransactionClicked();
    // relabels the extra field to match the selected transaction type
    void onTypeChanged(const QString& type);

private:
    Ui::ExpenseTrackerWindow* ui;
};

#endif //EXPENSETRACKER_GUI_H
