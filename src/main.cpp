#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    application.setApplicationName("Virtual Camera Manager");
    application.setApplicationVersion("2.0");

    MainWindow window;
    window.show();

    return application.exec();
}
