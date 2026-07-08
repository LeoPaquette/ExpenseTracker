#include "../include/GUI.h"

#include <QApplication>

// creates the qt application
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    GUI window;
    window.show();

    return QApplication::exec();
}
