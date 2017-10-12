#include "multi_track_media_player.h"

#include <QDebug>

namespace _TEST {

MultiTrackMediaPlayer::MultiTrackMediaPlayer(QWidget* parent)
    : QWidget(parent)
    , active_players_()
    , active_widgets_()
    , widget_layout_(0)
{
    initLayout();
}

MultiTrackMediaPlayer::~MultiTrackMediaPlayer()
{
    removeAll();
}

const QMediaContent MultiTrackMediaPlayer::media(int id) const
{
    if(active_players_.contains(id))
        return active_players_[id]->media();
    else {
        qDebug() << "FAILURE: Player with ID"<< id << "does not exist.";
        return QMediaContent();
    }
}

void MultiTrackMediaPlayer::addTrack(int id)
{
    addPlayer(id);
    addWidget(id);
}

void MultiTrackMediaPlayer::removeTrack(int id)
{
    removePlayer(id);
    removeWidget(id);
}

void MultiTrackMediaPlayer::setMedia(int id, const QMediaContent &content, QIODevice *device)
{
    if(active_players_.contains(id))
        active_players_[id]->setMedia(content, device);
    else
        qDebug() << "FAILURE: Player with ID"<< id << "does not exist.";
}

void MultiTrackMediaPlayer::setVolume(int id, int volume)
{
    if(active_players_.contains(id))
        active_players_[id]->setVolume(volume);
    else
        qDebug() << "FAILURE: Player with ID"<< id << "does not exist.";
}

void MultiTrackMediaPlayer::play(int id)
{
    if(active_players_.contains(id))
        active_players_[id]->play();
    else
        qDebug() << "FAILURE: Player with ID"<< id << "does not exist.";
}

void MultiTrackMediaPlayer::stop(int id)
{
    if(active_players_.contains(id))
        active_players_[id]->stop();
    else
        qDebug() << "FAILURE: Player with ID"<< id << "does not exist.";
}

void MultiTrackMediaPlayer::pause(int id)
{
    if(active_players_.contains(id))
        active_players_[id]->pause();
    else
        qDebug() << "FAILURE: Player with ID"<< id << "does not exist.";
}

void MultiTrackMediaPlayer::addPlayer(int id)
{
    if(active_players_.contains(id)) {
        qDebug() << "NOTIFICATION: Player with ID" << id << "already exists.";
        return;
    }

    active_players_.insert(id, new QMediaPlayer(this));

    // set start volume of media player to 50%
    setVolume(id, 50);
}

void MultiTrackMediaPlayer::removePlayer(int id)
{
    if(!active_players_.contains(id)) {
        qDebug() << "NOTIFICATION: Player with ID" << id << "does not exist.";
        qDebug() << " > No player to remove.";
        return;
    }

    active_players_[id]->stop();
    delete active_players_[id];
    active_players_.remove(id);
}

void MultiTrackMediaPlayer::addWidget(int id)
{
    if(active_widgets_.contains(id)) {
        qDebug() << "NOTIFICATION: Widget with ID" << id << "already exists.";
        return;
    }

    // make sure player exists for connections below
    if(!active_players_.contains(id))
        addPlayer(id);

    AudioWidget* widget = new AudioWidget(this, id);
    active_widgets_.insert(id, widget);

    widget_layout_->addWidget(active_widgets_[id]);

    connect(active_widgets_[id], SIGNAL(contentChanged(int,QMediaContent)),
            this, SLOT(setMedia(int,QMediaContent)));
    connect(active_widgets_[id], SIGNAL(play(int)),
            this, SLOT(play(int)));
    connect(active_widgets_[id], SIGNAL(stop(int)),
            this, SLOT(stop(int)));
    connect(active_widgets_[id], SIGNAL(pause(int)),
            this, SLOT(pause(int)));
    connect(active_widgets_[id], SIGNAL(changeVolume(int,int)),
            this, SLOT(setVolume(int,int)));
    connect(active_widgets_[id], SIGNAL(closed(int)),
            this, SLOT(removeTrack(int)));
}

void MultiTrackMediaPlayer::removeWidget(int id)
{
    if(!active_widgets_.contains(id))
        return;

    widget_layout_->removeWidget(active_widgets_[id]);
    delete active_widgets_[id];
    active_widgets_.remove(id);
}

void MultiTrackMediaPlayer::removeAll()
{
    while(active_players_.size() > 0)
        removePlayer(active_players_.begin().key());
    while(active_widgets_.size() > 0)
        removeWidget(active_widgets_.begin().key());
}


void MultiTrackMediaPlayer::initLayout()
{
    widget_layout_ = new QVBoxLayout;
    widget_layout_->setAlignment(Qt::AlignTop);
    setLayout(widget_layout_);
}

} // namespace _TEST
