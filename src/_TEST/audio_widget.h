#ifndef TEST_AUDIO_WIDGET_H
#define TEST_AUDIO_WIDGET_H

#include <QWidget>

#include <QPushButton>
#include <QMediaContent>
#include <QGroupBox>

#include "player_controls.h"
#include "content_browser.h"

namespace _TEST {

/*
 * Defines a UI for interaction with a audio content.
 * Holds the QMediaContent for the associated file.
 **/
class AudioWidget : public QWidget
{
    Q_OBJECT

public:
    AudioWidget(QWidget* parent = 0, int id = -1);

    int getID() const;
    int getVolume() const;

signals:
    void play();
    void stop();
    void pause();
    void changeVolume(int);
    void contentChanged(QMediaContent const&);
    void closed();

    void play(int);
    void stop(int);
    void pause(int);
    void changeVolume(int,int);
    void contentChanged(int,QMediaContent const&);
    void closed(int);

private slots:
    void onPlayClicked(bool);
    void onStopClicked(bool);
    void onPauseClicked(bool);
    void onUrlChanged(QUrl const&);
    void onChangeVolume(int);
    void onClosedClicked(bool);

private:
    void initWidgets();
    void initLayout();

    int id_;
    QMediaContent content_;
    PlayerControls* controls_;
    ContentBrowser* content_browser_;
    QPushButton* close_button_;
    QGroupBox* box_;
};

} // namespace _TEST

#endif // TEST_AUDIO_WIDGET_H
