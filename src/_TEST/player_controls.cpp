#include "player_controls.h"

#include <QHBoxLayout>

namespace _TEST {

PlayerControls::PlayerControls(QWidget* parent)
    : QWidget(parent)
    , play_button_(0)
    , stop_button_(0)
    , pause_button_(0)
    , volume_slider_(0)
{
    initWidgets();
    initLayout();
}

PlayerControls::~PlayerControls()
{}

void PlayerControls::disable()
{
    play_button_->setEnabled(false);
    play_button_->setHidden(false);

    pause_button_->setEnabled(false);
    pause_button_->setHidden(true);

    stop_button_->setEnabled(false);

    volume_slider_->setEnabled(false);
}

void PlayerControls::enable()
{
    play_button_->setEnabled(true);
    play_button_->setHidden(false);

    pause_button_->setEnabled(false);
    pause_button_->setHidden(true);

    stop_button_->setEnabled(false);

    volume_slider_->setEnabled(true);
}

int PlayerControls::getVolume() const
{
    return volume_slider_->value();
}

void PlayerControls::onPlayClicked(bool click)
{
    play_button_->setEnabled(false);
    play_button_->setHidden(true);

    pause_button_->setEnabled(true);
    pause_button_->setHidden(false);

    stop_button_->setEnabled(true);

    emit playClicked(click);
}

void PlayerControls::onStopClicked(bool click)
{
    play_button_->setEnabled(true);
    play_button_->setHidden(false);

    pause_button_->setEnabled(false);
    pause_button_->setHidden(true);

    stop_button_->setEnabled(false);

    emit stopClicked(click);
}

void PlayerControls::onPauseClicked(bool click)
{
    play_button_->setEnabled(true);
    play_button_->setHidden(false);

    pause_button_->setEnabled(false);
    pause_button_->setHidden(true);

    stop_button_->setEnabled(true);

    emit pauseClicked(click);
}

void PlayerControls::initWidgets()
{
    play_button_ = new QPushButton("play", this);
    stop_button_ = new QPushButton("stop", this);
    pause_button_ = new QPushButton("pause", this);

    volume_slider_ = new QSlider(Qt::Horizontal, this);
    volume_slider_->setMinimum(0);
    volume_slider_->setMaximum(100);
    volume_slider_->setSliderPosition(50);

    connect(play_button_, SIGNAL(clicked(bool)),
            this, SLOT(onPlayClicked(bool)));
    connect(stop_button_, SIGNAL(clicked(bool)),
            this, SLOT(onStopClicked(bool)));
    connect(pause_button_, SIGNAL(clicked(bool)),
            this, SLOT(onPauseClicked(bool)));
    connect(volume_slider_, SIGNAL(valueChanged(int)),
            this, SIGNAL(volumnChanged(int)));

    enable();
}

void PlayerControls::initLayout()
{
    QHBoxLayout* layout = new QHBoxLayout;

    layout->addWidget(play_button_, 1);
    layout->addWidget(pause_button_, 1);
    layout->addWidget(stop_button_, 1);
    layout->addWidget(volume_slider_, 3);

    setLayout(layout);
}

} // namespace _TEST
