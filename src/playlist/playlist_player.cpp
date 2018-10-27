#include "playlist_player.h"

PlaylistPlayer::PlaylistPlayer(QObject* parent)
    : QMediaPlayer(parent)
    , activated_(false)
    , current_content_index_(0)
    , delay_flag_(false)
    , delay_(0)
    , delay_timer_(0)
{
    delay_timer_ = new QTimer(this);
    connect(delay_timer_, SIGNAL(timeout()),
            this, SLOT(onDelayIsOver()));
}

void PlaylistPlayer::play()
{

    Playlist* playlist = getPlaylist();
    if(playlist) {
        PlaylistSettings settings = playlist->getSettings();
        setVolume(settings.volume);
        // if delay interval is turned on
        if (settings.order == PlayOrder::ORDERED){
            if (settings.loop_flag){
                playlist->setPlaybackMode(QMediaPlaylist::Loop);

            }else if (!settings.loop_flag){
                playlist->setPlaybackMode(QMediaPlaylist::Sequential);

            }
        } else if (settings.order == PlayOrder::SHUFFLE){
            playlist->setPlaybackMode(QMediaPlaylist::Random);
            int index = getRandomIntInRange(0, playlist->mediaCount()-1);
            playlist->setCurrentIndex(index);

        } else if (settings.order == PlayOrder::WEIGHTED){
            // TODO implement weighted
            QMediaPlayer::pause();
        }

        if (settings.interval_flag){
            delay_ = getRandomIntInRange(settings.min_delay_interval,
                                         settings.max_delay_interval);
            delay_flag_ = true;
            if (activated_){
                QMediaPlayer::play();
                playlist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
            }
        } else {
            delay_flag_ = false;
            delay_ = 0;
            if (activated_){
                QMediaPlayer::play();
            }
        }
    }
}

void PlaylistPlayer::setPlaylist(Playlist *playlist)
{
    connect(playlist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onCurrentMediaIndexChanged(int)) );

    connect(playlist, SIGNAL(changedSettings()),
            this, SLOT(onMediaSettingsChanged()) );

    QMediaPlayer::setPlaylist(playlist);
}

void PlaylistPlayer::onDelayIsOver()
{
    delay_timer_->stop();
    play();
}

void PlaylistPlayer::activate()
{
    activated_ = true;
    emit playerActivationToggled(true);
}

void PlaylistPlayer::deactivate()
{
    activated_ = false;
    emit playerActivationToggled(false);
}

void PlaylistPlayer::setActivation(bool flag)
{
    activated_ = flag;
    emit playerActivationToggled(flag);
}

int PlaylistPlayer::getRandomIntInRange(int min, int max)
{
    int range = max - min;
    return (qrand() % (range+1)) + min;
}

void PlaylistPlayer::onCurrentMediaIndexChanged(int position)
{
    current_content_index_ = position;
    if (activated_ && delay_flag_){
        delay_timer_->start(delay_*1000);
    }
}

void PlaylistPlayer::onMediaSettingsChanged()
{
    PlaylistSettings settings = getPlaylist()->getSettings();
    setVolume(settings.volume);
    if (settings.interval_flag){
        delay_flag_ = true;
        delay_ = getRandomIntInRange(settings.min_delay_interval,
                                     settings.max_delay_interval);
    } else {
        delay_flag_ = false;
        delay_ = 0;
    }

    if (settings.order == PlayOrder::ORDERED){

    } else if (settings.order == PlayOrder::SHUFFLE){
        getPlaylist()->setPlaybackMode(QMediaPlaylist::Random);
    } else if (settings.order == PlayOrder::WEIGHTED){
        qDebug() << "weigthed not implemented yet";
    }
}

void PlaylistPlayer::onMediaVolumeChanged(int val)
{
    if (val >= 0 && val <= 100)
        setVolume(val);
}


Playlist *PlaylistPlayer::getPlaylist() const
{

    Playlist* pl = qobject_cast<Playlist*>( playlist() );
    if(pl){
        return pl;
    } else {
        return nullptr;
    }
}

