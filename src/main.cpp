#include "main_window.h"

#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QSettings>

#include "resources/lib.h"
#include "log/file_logger.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Resources::Lib::init();
    a.setStyleSheet(Resources::Lib::DARK_STYLE);

    Log::FileLogger::clearLogFile();

    MainWindow w;
    w.showMaximized();

    int ret_code = a.exec();

    Resources::Lib::cleanup();

    return ret_code;
}
