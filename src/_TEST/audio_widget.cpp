#include "audio_widget.h"

#include <QHBoxLayout>
#include <QDebug>

namespace _TEST {

AudioWidget::AudioWidget(QWidget* parent, int id)
    : QWidget(parent)
    , id_(id)
    , content_()
    , controls_(0)
    , content_browser_(0)
    , close_button_(0)
    , box_(0)
{
    initWidgets();
    initLayout();
}

int AudioWidget::getID() const
{
    return id_;
}

int AudioWidget::getVolume() const
{
    return controls_->getVolume();
}

void AudioWidget::onPlayClicked(bool)
{
    emit play();
    emit play(id_);
}

void AudioWidget::onStopClicked(bool)
{
    emit stop();
    emit stop(id_);
}

void AudioWidget::onPauseClicked(bool)
{
    emit pause();
    emit pause(id_);
}

void AudioWidget::onUrlChanged(const QUrl& url)
{
    if(!url.isEmpty() && url.isValid()) {
        content_ = QMediaContent(url);
        emit contentChanged(content_);
        emit contentChanged(id_, content_);
    }
}

void AudioWidget::onChangeVolume(int vol)
{
    emit changeVolume(vol);
    emit changeVolume(id_, vol);
}

void AudioWidget::onClosedClicked(bool)
{
    emit closed();
    emit closed(id_);
}

void AudioWidget::initWidgets()
{
    controls_ = new PlayerControls(this);
    content_browser_ = new ContentBrowser(this);
    close_button_ = new QPushButton("x", this);
    box_ = new QGroupBox(this);

    box_->setTitle(tr("Track ") + QString::number(id_));

    connect(controls_, SIGNAL(playClicked(bool)),
            this, SLOT(onPlayClicked(bool)));
    connect(controls_, SIGNAL(stopClicked(bool)),
            this, SLOT(onStopClicked(bool)));
    connect(controls_, SIGNAL(pauseClicked(bool)),
            this, SLOT(onPauseClicked(bool)));
    connect(controls_, SIGNAL(volumnChanged(int)),
            this, SLOT(onChangeVolume(int)));
    connect(content_browser_, SIGNAL(urlChanged(QUrl)),
            this, SLOT(onUrlChanged(QUrl)));
    connect(close_button_, SIGNAL(clicked(bool)),
            this, SLOT(onClosedClicked(bool)));
}

void AudioWidget::initLayout()
{
    QHBoxLayout* layout = new QHBoxLayout;

    QHBoxLayout* box_layout = new QHBoxLayout;

    box_layout->addWidget(controls_, 1);
    box_layout->addWidget(content_browser_, 1);
    box_layout->addStretch(1);
    box_layout->addWidget(close_button_, -1);

    box_->setLayout(box_layout);

    layout->addWidget(box_);

    setLayout(layout);
}

} // namespace _TEST
