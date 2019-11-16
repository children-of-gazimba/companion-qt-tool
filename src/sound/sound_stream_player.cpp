#include "sound_stream_player.h"
#include "resources/lib.h"

#include <QDateTime>
#include <QHBoxLayout>
#include <QMediaPlaylist>

SoundStreamPlayer::SoundStreamPlayer(QWidget *parent)
    : QWidget(parent)
    , player_(0)
    , playlist_(0)
    , play_button_(0)
    , current_sound_(0)
    , progress_(0)
    , current_time_(0)
    , total_time_(0)
    , is_playing_(false)
    , play_icon_()
    , pause_icon_()
    , ignore_progress_change_(false)
    , repo_(new SoundRepository(this))
{
    initWidgets();
    initLayout();
}

SoundStreamPlayer::~SoundStreamPlayer()
{}

void SoundStreamPlayer::setSoundFile(const SoundData &sound, bool auto_play)
{
    resetPlayer();
    current_sound_->setText(sound.resource.name);
    QUrl url(repo_->getStreamUrl(sound));
    if(url.isValid()) {
        playlist_->addMedia(url);
    }
    current_time_->setText("0:00");
    play_button_->setEnabled(playlist_->mediaCount() > 0);
    if(auto_play)
        play();
}

void SoundStreamPlayer::play()
{
    if(is_playing_)
        return;
    player_->play();
    is_playing_ = true;
    play_button_->setIcon(pause_icon_);
}

void SoundStreamPlayer::pause()
{
    if(!is_playing_)
        return;
    player_->pause();
    is_playing_ = false;
    play_button_->setIcon(play_icon_);
}

void SoundStreamPlayer::stop()
{
    player_->stop();
    is_playing_ = false;
    play_button_->setIcon(play_icon_);
}

void SoundStreamPlayer::onPlayButtonClicked()
{
    if(is_playing_)
        player_->pause();
    else
        player_->play();
    is_playing_ = !is_playing_;
    play_button_->setIcon(is_playing_ ? pause_icon_ : play_icon_);
}

void SoundStreamPlayer::onPlaybackPositionChanged(qint64 v)
{
    int p = qRound((v / (float) player_->duration()) * 100);
    ignore_progress_change_ = true;
    progress_->setValue(p);
    current_time_->setText(timeString(v));
}

void SoundStreamPlayer::onPlaybackDurationChanged(qint64 v)
{
    total_time_->setText(timeString(v));
}

void SoundStreamPlayer::onProgressChanged(int v)
{
    if(ignore_progress_change_) {
        ignore_progress_change_ = false;
        return;
    }
    qint64 p = qRound((v * player_->duration()) / 100.0f);
    player_->setPosition(p);
}

void SoundStreamPlayer::resetPlayer()
{
    if(player_->isAudioAvailable()) {
        player_->stop();
        if(playlist_->mediaCount() > 0)
            playlist_->clear();
        play_button_->setIcon(play_icon_);
        is_playing_ = false;
    }
}

void SoundStreamPlayer::initWidgets()
{
    repo_->loadApiTokenFromJsonFile();

    play_icon_ = QIcon(*Resources::Lib::PX_PLAY);
    pause_icon_ = QIcon(*Resources::Lib::PX_PAUSE);

    player_ = new QMediaPlayer(this);
    playlist_ = new QMediaPlaylist(player_);
    playlist_->setPlaybackMode(QMediaPlaylist::Loop);
    player_->setPlaylist(playlist_);
    connect(player_, &QMediaPlayer::positionChanged,
            this, &SoundStreamPlayer::onPlaybackPositionChanged);
    connect(player_, &QMediaPlayer::durationChanged,
            this, &SoundStreamPlayer::onPlaybackDurationChanged);

    play_button_ = new QPushButton(this);
    play_button_->setEnabled(false);
    play_button_->setIcon(play_icon_);
    connect(play_button_, &QPushButton::clicked,
            this, &SoundStreamPlayer::onPlayButtonClicked);

    progress_ = new QSlider(Qt::Horizontal, this);
    progress_->setMinimumSize(progress_->sizeHint());
    connect(progress_, &QSlider::valueChanged,
            this, &SoundStreamPlayer::onProgressChanged);

    current_sound_ = new QLabel(tr("[select a track]"), this);
    current_sound_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    current_time_ = new QLabel("0:00", this);
    current_time_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    total_time_ = new QLabel("0:00", this);
    total_time_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

void SoundStreamPlayer::initLayout()
{
    QHBoxLayout* l = new QHBoxLayout;
    l->addWidget(play_button_, 0);
    l->addWidget(current_sound_, 3, Qt::AlignHCenter | Qt::AlignVCenter);
    l->addWidget(current_time_, -1, Qt::AlignRight | Qt::AlignVCenter);
    l->addWidget(progress_, 3);
    l->addWidget(total_time_, -1, Qt::AlignLeft | Qt::AlignVCenter);
    setLayout(l);
}

const QString SoundStreamPlayer::timeString(qint64 milliseconds)
{
    return QDateTime::fromTime_t(qRound(milliseconds/1000.0f)).toUTC().toString("mm:ss");
}
