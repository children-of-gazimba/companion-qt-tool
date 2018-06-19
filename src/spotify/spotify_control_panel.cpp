#include "spotify_control_panel.h"

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLabel>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QWidget>
#include <QSlider>

#include "spotify/spotify_handler.h"

SpotifyControlPanel::SpotifyControlPanel(QWidget *parent)
    : QWidget(parent)
{
    initWidgets();
    initLayout();

    // grant access to spotify
    if(!SpotifyHandler::isAccessGranted())
        SpotifyHandler::requestGrantAccess();
    else
        onAccessGranted();

    setWindowTitle("Spotify Control Panel");
    setWindowFlags(Qt::Tool);
}

void SpotifyControlPanel::initLayout()
{

    QVBoxLayout *button_layout = new QVBoxLayout;
    button_layout->addWidget(btn_ctrl_play_);
    button_layout->addWidget(btn_ctrl_pause_);
    button_layout->addWidget(btn_ctrl_next_);
    button_layout->addWidget(btn_ctrl_prev_);
    button_layout->addWidget(btn_ctrl_enable_shuffle);
    button_layout->addWidget(btn_ctrl_disable_shuffle);
    button_layout->addWidget(btn_ctrl_set_repeat_track);
    button_layout->addWidget(btn_ctrl_set_repeat_context);
    button_layout->addWidget(btn_ctrl_set_repeat_off);

    QVBoxLayout *volume_layout = new QVBoxLayout;
    volume_slider_ = new QSlider(Qt::Orientation::Horizontal, this);
    volume_layout->addWidget(volume_slider_, 10);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(button_layout);
    layout->addLayout(volume_layout);

    setLayout(layout);
}


QPushButton* SpotifyControlPanel::createButton(const QString &title, bool enabled, QWidget *parent)
{
    QPushButton *button = new QPushButton(title, parent);
    button->setEnabled(enabled);
    return button;
}

void SpotifyControlPanel::initWidgets()
{
    btn_ctrl_play_ = createButton("Play", false, this);
    btn_ctrl_pause_ = createButton("Pause", false, this);
    btn_ctrl_next_= createButton(">>", false, this);
    btn_ctrl_prev_= createButton("<<" , false, this);
    btn_ctrl_enable_shuffle= createButton("Enable Shuffle", false, this);
    btn_ctrl_disable_shuffle= createButton("Disable Shuffle", false, this);
    btn_ctrl_set_repeat_track= createButton("Repeat Track", false, this);
    btn_ctrl_set_repeat_context= createButton("Repeat Context", false, this);
    btn_ctrl_set_repeat_off= createButton("Repeat Off", false, this);

    connect(&SpotifyHandler::instance()->remote, &SpotifyRemoteController::accessGranted,
            this, &SpotifyControlPanel::onAccessGranted);
    connect(btn_ctrl_play_, &QPushButton::clicked,
            this, &SpotifyControlPanel::onPlay);
    connect(btn_ctrl_pause_, &QPushButton::clicked,
            this, &SpotifyControlPanel::onPause);
    connect(btn_ctrl_next_, &QPushButton::clicked,
            this, &SpotifyControlPanel::onNext);
    connect(btn_ctrl_prev_, &QPushButton::clicked,
            this, &SpotifyControlPanel::onPrev);
    connect(btn_ctrl_enable_shuffle, &QPushButton::clicked,
            this, &SpotifyControlPanel::onEnableShuffle);
    connect(btn_ctrl_disable_shuffle, &QPushButton::clicked,
            this, &SpotifyControlPanel::onDisableShuffle);
    connect(btn_ctrl_set_repeat_context, &QPushButton::clicked,
            this, &SpotifyControlPanel::onSetRepeatContext);
    connect(btn_ctrl_set_repeat_track, &QPushButton::clicked,
            this, &SpotifyControlPanel::onSetRepeatTrack);
    connect(btn_ctrl_set_repeat_off, &QPushButton::clicked,
            this, &SpotifyControlPanel::onSetRepeatOff);
    connect(volume_slider_, &QSlider::valueChanged,
            this, &SpotifyControlPanel::onSetVolume);
}

void SpotifyControlPanel::onAccessGranted()
{
    btn_ctrl_play_->setEnabled(true);
    btn_ctrl_pause_->setEnabled(true);
    btn_ctrl_next_->setEnabled(true);
    btn_ctrl_prev_->setEnabled(true);
    btn_ctrl_enable_shuffle->setEnabled(true);
    btn_ctrl_disable_shuffle->setEnabled(true);
    btn_ctrl_set_repeat_track->setEnabled(true);
    btn_ctrl_set_repeat_context->setEnabled(true);
    btn_ctrl_set_repeat_off->setEnabled(true);
}

void SpotifyControlPanel::onPlay()
{
    SpotifyHandler::instance()->remote.play();
}

void SpotifyControlPanel::onPause()
{
    SpotifyHandler::instance()->remote.pause();
}

void SpotifyControlPanel::onNext()
{
    SpotifyHandler::instance()->remote.next();
}

void SpotifyControlPanel::onPrev()
{
    SpotifyHandler::instance()->remote.prev();
}

void SpotifyControlPanel::onEnableShuffle()
{
    SpotifyHandler::instance()->remote.setShuffle(true);
}

void SpotifyControlPanel::onDisableShuffle()
{
    SpotifyHandler::instance()->remote.setShuffle(false);
}

void SpotifyControlPanel::onSetRepeatContext()
{
    SpotifyHandler::instance()->remote.setRepeat(SpotifyRemoteController::RepeatMode::Context);
}

void SpotifyControlPanel::onSetRepeatTrack()
{
    SpotifyHandler::instance()->remote.setRepeat(SpotifyRemoteController::RepeatMode::Track);
}

void SpotifyControlPanel::onSetRepeatOff()
{
    SpotifyHandler::instance()->remote.setRepeat(SpotifyRemoteController::RepeatMode::Off);
}

void SpotifyControlPanel::onSetVolume(int value)
{
    SpotifyHandler::instance()->remote.setVolume(value);
}
