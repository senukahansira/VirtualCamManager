#include <QApplication>
#include "MainWindow.h"

/*
 * main()
 *
 * This is where our C++ program starts.
 *
 * QApplication creates the Qt application.
 * MainWindow creates our actual GUI.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Create our main application window.
    MainWindow window;

    // Show the window on the screen.
    window.show();

    // Start Qt's event loop.
    // This keeps the application running and allows
    // buttons, mouse clicks, etc. to work.
    return app.exec();
}