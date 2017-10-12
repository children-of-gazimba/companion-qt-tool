#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QList>
#include <QMap>
#include <QAction>

#include "dsa_media_control_kit.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void toggleFullScreen(bool);

private:
    void initCentralWidget();
    void initActions();
    void initMenuBar();
    void initStatusBar();

    DsaMediaControlKit* kit_;
    QMap<QString, QList<QAction*> > actions_;
};

#endif // MAIN_WINDOW_H
