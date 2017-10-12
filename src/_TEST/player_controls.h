#ifndef TEST_CONTENT_MEDIA_CONTROLS_H
#define TEST_CONTENT_MEDIA_CONTROLS_H

#include <QWidget>
#include <QPushButton>
#include <QSlider>

namespace _TEST {

/*
 * Defines UI for basic player controls.
 * Play-, Pause-, Stop button, Volume slider.
*/
class PlayerControls : public QWidget
{
    Q_OBJECT
public:
    PlayerControls(QWidget* parent = 0);
    ~PlayerControls();

    /* disables interaction with UI **/
    void disable();
    /* enabled interaction with UI **/
    void enable();

    int getVolume() const;

signals:
    void playClicked(bool);
    void stopClicked(bool);
    void pauseClicked(bool);
    void volumnChanged(int);

private slots:
    void onPlayClicked(bool);
    void onStopClicked(bool);
    void onPauseClicked(bool);

private:
    void initWidgets();
    void initLayout();

    QPushButton* play_button_;
    QPushButton* stop_button_;
    QPushButton* pause_button_;
    QSlider* volume_slider_;
};

} // namespace _TEST

#endif // TEST_CONTENT_MEDIA_CONTROLS_H
