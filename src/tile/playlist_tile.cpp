#include "playlist_tile.h"
#include "resources/lib.h"

#include <QGraphicsScene>
#include <QDebug>
#include <QMenu>
#include <QJsonArray>

#include "sound/sound_list_view_dialog.h"
#include "misc/volume_mapper.h"

namespace Tile {

PlaylistTile::PlaylistTile(QGraphicsItem *parent)
    : BaseTile(parent)
    , player_(0)
    , playlist_settings_widget_(0)
    , playlist_(0)
    , model_(0)
    , is_playing_(false)
    , draw_filled_volume_indicator_(false)
    , filled_volume_timer_()
{
    player_ = new PlaylistPlayer(this);
    //connect(player_, SIGNAL(stateChanged(QMediaPlayer::State)),
    //        this, SLOT(changePlayerState(QMediaPlayer::State)));

    connect(player_, SIGNAL(playerActivationToggled(bool)),
            this, SLOT(changedCustomPlayerActivation(bool)));

    filled_volume_timer_.setSingleShot(true);
    connect(&filled_volume_timer_, &QTimer::timeout,
            this, [=]() {
        prepareGeometryChange();
        draw_filled_volume_indicator_ = false;
    });

    playlist_ = new Playlist("Playlist");
    player_->setPlaylist(playlist_);
    setAcceptDrops(true);
}

PlaylistTile::~PlaylistTile()
{
    playlist_->deleteLater();
    if(playlist_settings_widget_)
        playlist_settings_widget_->deleteLater();
}


void PlaylistTile::paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    BaseTile::paint(painter, option, widget);

    QRectF p_rect(getPaintRect());
    if(p_rect.width() > 0 && p_rect.height() > 0) {
        // draw current volume
        QRectF volume_rect(getVolumeRect());
        QLineF volume_line(volume_rect.topLeft(), volume_rect.topRight());
        qreal op = painter->opacity();
        painter->setOpacity(0.5);
        painter->setPen(QPen(QBrush(Qt::green), 5));
        painter->drawLine(volume_line);
        if(draw_filled_volume_indicator_)
            painter->fillRect(volume_rect, Qt::green);
        painter->setOpacity(op);

        // draw play state
        painter->drawPixmap(
            (int) p_rect.x(),
            (int)p_rect.y(),
            (int)p_rect.width(),
            (int)p_rect.height(),
            getPlayStatePixmap()
        );
    }

    /*
    int y= 0;
    //foreach(const QMediaContent* media,playlist_->media())
    for (int i = 0; i < playlist_->mediaCount(); i++)
    {
        QMediaContent c =playlist_->media(i);
        QUrl url = c.canonicalUrl();
        painter->drawText(QPointF(p_rect.x(), p_rect.y()+size_+y),url.fileName());
        y += 10;
    }
    */

}

void PlaylistTile::receiveExternalData(const QMimeData *data)
{
    auto sounds = JsonMimeDataParser::toSoundList(data);
    if(sounds.size() == 0)
        return;

    foreach(auto sound, sounds)
        addMedia(QualifiedSoundData(sound, "local"));
}


void PlaylistTile::receiveWheelEvent(QWheelEvent *event)
{
    Playlist* pl = player_->getPlaylist();
    PlaylistSettings settings = pl->getSettings();

    int log_volume = VolumeMapper::linearToLogarithmic(settings.volume);
    if (event->delta() < 0) {
        log_volume -= 3;
        if (log_volume < 0)
            log_volume = 0;
    }
    else if(event->delta() >= 0) {
        log_volume += 3;
        if (log_volume > 100)
            log_volume = 100;
    }

    int new_volume = VolumeMapper::logarithmicToLinear(log_volume);
    if(new_volume != settings.volume) {
        setVolume(new_volume);
        emit wheelChangedVolume(settings.volume);
    }
}

bool PlaylistTile::addMedia(const QualifiedSoundData& sound)
{
    return playlist_->addMedia(sound);
}

void PlaylistTile::setSoundFileModel(SoundFileTableModel *m)
{
    model_ = m;
    //playlist_->setSoundFileModel(model_);
}

SoundFileTableModel *PlaylistTile::getSoundFileModel()
{
    return model_;
}

const QJsonObject PlaylistTile::toJsonObject() const
{
    QJsonObject obj = BaseTile::toJsonObject();

    // store playlist
    QJsonArray arr_pl;
    foreach(auto sound, playlist_->getSounds())
        arr_pl.append(sound.toJsonObject());
    obj["playlist"] = arr_pl;

    //store settings
    QJsonObject obj_settings;
    obj_settings = JsonMimeDataParser::toJsonObject(playlist_->getSettings());
    obj["settings"] = obj_settings;

    return obj;
}

bool PlaylistTile::setFromJsonObject(const QJsonObject &obj)
{
    return false;
    /*if(!BaseTile::setFromJsonObject(obj))
        return false;

    // parse playlist
    if(obj.contains("playlist") && obj["playlist"].isArray()) {
        foreach(QJsonValue val, obj["playlist"].toArray()) {
            QJsonObject sound_obj = val.toObject();
            if(sound_obj.isEmpty())
                continue;

            TableRecord* rec = JsonMimeDataParser::toTableRecord(sound_obj);
            if(rec->index != SOUND_FILE) {
                delete rec;
                continue;
            }

            // check existance against actual database
            SoundFileRecord* sf_rec = (SoundFileRecord*) rec;
            QList<SoundFileRecord*> actual_recs = model_->getSoundFilesByRelativePath(sf_rec->relative_path);
            if(actual_recs.size() == 0) {
                qDebug() << "FAILURE: Could not verify SoundFile existance.";
                qDebug() << " > SoundFile:" << sound_obj << "does not exist in any ResourceDirectory.";
                qDebug() << " > Make sure relative path (" << sf_rec->relative_path << ") exists within a ResourceDirectory.";
                sf_rec = 0;
                delete rec;
                return false;
            }
            else if(actual_recs.size() > 1) {
                bool found = false;
                foreach(SoundFileRecord* act_rec, actual_recs) {
                    if(act_rec->id == sf_rec->id) {
                        sf_rec->copyFrom(act_rec);
                        found = true;
                        break;
                    }
                }

                if(!found) {
                    qDebug() << "NOTIFICATION: More than one SoundFile exists for relative path";
                    qDebug() << " > And ID of SoundFiles parsed from JSON cannot be found in database.";
                    qDebug() << " > automatically picking first matched SoundFile.";
                    sf_rec->copyFrom(actual_recs[0]);
                }


            }
            else { // exactly one SoundFIle matches
                sf_rec->copyFrom(actual_recs[0]);
            }

            bool success = playlist_->addMedia(*sf_rec);
            if(!success) {
                qDebug() << "FAILURE: Could not add SoundFile from JSON";
                qDebug() << " > " << sound_obj;
                sf_rec = 0;
                delete rec;
                return false;
            }

            sf_rec = 0;
            delete rec;
        }
    }

    // parse settings
    if(obj.contains("settings") && obj["settings"].isObject() ) {
        QJsonObject s_obj = obj["settings"].toObject();
        if(s_obj.isEmpty())
            return false;
        //to do: no pointer needed
        PlaylistSettings* settings = JsonMimeDataParser::toPlaylistSettings(s_obj);
        if(!settings) {
            qDebug() << "FAILURE: Could not set Playlist Settings from JSON";
            qDebug() << " > " << s_obj;
        }
        playlist_->setSettings(*settings);
        delete settings;
        settings = 0;
    }

    return true;*/
}

void PlaylistTile::setMedia(const QMediaContent &c)
{
    player_->setMedia(c);
}

void PlaylistTile::play()
{
    if(!player_->media().isNull() && !is_playing_) {
        player_->activate();
        player_->play();
        setIsPlaying(true);
    }
}

void PlaylistTile::stop()
{
    if(!player_->media().isNull() && is_playing_) {
        player_->stop();
        player_->deactivate();
        setIsPlaying(false);
    }
}

void PlaylistTile::onActivate()
{
    if(is_playing_)
        stop();
    else
        play();

    BaseTile::onActivate();
}

void PlaylistTile::setVolume(int volume)
{
    PlaylistSettings settings = player_->getPlaylist()->getSettings();
    settings.volume = volume;
    player_->getPlaylist()->setSettings(settings);
    volumeChangedEvent();
    //player_->setVolume(volume);
}

int PlaylistTile::getVolume() const
{
    return player_->volume();
}

void PlaylistTile::changePlayerState(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::PlayingState){
        setIsPlaying(true);
    } else if (state == QMediaPlayer::StoppedState){
        setIsPlaying(false);
    }
}

void PlaylistTile::changedCustomPlayerActivation(bool state)
{
    if (state == true){
        setIsPlaying(true);
    } else if (state == false){
        setIsPlaying(false);
    }
}

void PlaylistTile::onConfigurePlaylist()
{
    PlaylistSettings s = playlist_->getSettings();
    s.name = getName();
    if(!playlist_settings_widget_) {
        playlist_settings_widget_ = new PlaylistSettingsWidget(s);
        playlist_settings_widget_->setWindowTitle(tr("Playback Settings"));
        playlist_settings_widget_->move(QCursor::pos() - QPoint(170,170));
        connect(playlist_settings_widget_, &PlaylistSettingsWidget::closed,
                this, &PlaylistTile::closePlaylistSettings);
        connect(playlist_settings_widget_, &PlaylistSettingsWidget::saved,
                this, &PlaylistTile::savePlaylistSettings);
        connect(playlist_settings_widget_, &PlaylistSettingsWidget::volumeSettingsChanged,
                this, &PlaylistTile::setVolume);
        connect(this, &PlaylistTile::wheelChangedVolume,
                playlist_settings_widget_, &PlaylistSettingsWidget::onExternalVolumeChanged);
    }
    playlist_settings_widget_->popOpen();
}

void PlaylistTile::onContents()
{
    /*SoundListViewDialog d(playlist_->getSounds());
    d.setAcceptDrops(false);

    if(d.exec()) {

    }*/
}

void PlaylistTile::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    if(mode_ != MOVE && e->button() == Qt::LeftButton) {
        if(e->modifiers() & Qt::ControlModifier)
            return BaseTile::mouseReleaseEvent(e);
        if(is_playing_)
            stop();
        else
            play();
    }
    BaseTile::mouseReleaseEvent(e);
}

void PlaylistTile::closePlaylistSettings()
{
    playlist_settings_widget_->deleteLater();
    playlist_settings_widget_ = 0;
}

void PlaylistTile::savePlaylistSettings(const PlaylistSettings &settings)
{
    if(settings.name.size() > 0 && name_.compare(settings.name) != 0)
        setName(settings.name);
    if(settings.image_path.size() > 0)
        loadOverlayPixmap(settings.image_path);
    else
        clearOverlayPixmap();

    PlaylistSettings copied_settings = playlist_->getSettings();
    int master = copied_settings.master;
    copied_settings.copyFrom(settings);
    copied_settings.master = master;

    playlist_->setSettings(copied_settings);
    playlist_settings_widget_->hide();
    playlist_settings_widget_->deleteLater();
    playlist_settings_widget_ = nullptr;
}

void PlaylistTile::createContextMenu()
{
    // create configure action
    QAction* configure_action = new QAction(tr("Configure..."),this);

    connect(configure_action, SIGNAL(triggered()),
            this, SLOT(onConfigurePlaylist()));

    QAction* contents_action = new QAction(tr("Contents..."),this);

    connect(contents_action, SIGNAL(triggered()),
            this, SLOT(onContents()));

    context_menu_->addAction(configure_action);
    context_menu_->addAction(contents_action);
    context_menu_->addSeparator();

    BaseTile::createContextMenu();
}

const QPixmap PlaylistTile::getPlayStatePixmap() const
{
    if(is_playing_)
        return *Resources::Lib::PX_STOP;
    else
        return *Resources::Lib::PX_PLAY;
}

void PlaylistTile::masterScaleChangedEvent(float old_master)
{
    BaseTile::masterScaleChangedEvent(old_master);
    PlaylistSettings settings = player_->getPlaylist()->getSettings();
    int master_volume = static_cast<int>(master_scale_*100.0f);
    settings.master = VolumeMapper::logarithmicToLinear(master_volume);
    player_->getPlaylist()->setSettings(settings);
}

void PlaylistTile::volumeChangedEvent()
{
    prepareGeometryChange();
    draw_filled_volume_indicator_ = true;
    filled_volume_timer_.start(2500);
}

void PlaylistTile::setIsPlaying(bool state)
{
    is_playing_ = state;
}

const QRectF PlaylistTile::getVolumeRect() const
{
    int log_volume = player_->getPlaylist()->getSettings().volume;
    log_volume = VolumeMapper::linearToLogarithmic(log_volume);
    QRectF volume_rect(getPaintRect());
    volume_rect.setTop(volume_rect.bottom()-volume_rect.height()*(log_volume/100.0f));
    return volume_rect;
}

} // namespace Tile
