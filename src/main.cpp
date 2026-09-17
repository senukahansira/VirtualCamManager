#include <QApplication>

#include "MainWindow.h"

/*
 * Program entry point.
 *
 * Every C++ program starts execution from main().
 */
int main(int argc, char *argv[])
{
    /*
     * QApplication manages the Qt application.
     *
     * It handles things such as:
     *
     * - Windows
     * - Mouse
     * - Keyboard
     * - Events
     * - GUI rendering
     */
    QApplication app(argc, argv);

    /*
     * Create our main window.
     */
    MainWindow window;

    /*
     * Show the window.
     */
    window.show();

    /*
     * Start Qt's event loop.
     *
     * The program stays here waiting for:
     *
     * - Button clicks
     * - Mouse events
     * - Keyboard events
     * - QProcess signals
     * - etc.
     */
    return app.exec();
}