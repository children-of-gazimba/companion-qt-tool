#include "spotify_tile.h"
#include "resources/lib.h"

#include <QGraphicsScene>
#include <QDebug>
#include <QMenu>
#include <QJsonArray>
#include <QMessageBox>

#include "spotify/spotify_configure_dialog.h"

#include "spotify/spotify_handler.h"


namespace Tile {

SpotifyTile::SpotifyTile(QGraphicsItem *parent)
    : BaseTile(parent)
    , is_playing_(false)
{
    setAcceptDrops(true);
}

SpotifyTile::~SpotifyTile()
{ }


void SpotifyTile::paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    BaseTile::paint(painter, option, widget);

    QRectF p_rect(getPaintRect());
    if(p_rect.width() > 0 && p_rect.height() > 0) {
        painter->drawPixmap(
            (int) p_rect.x(),
            (int) p_rect.y(),
            (int) p_rect.width(),
            (int) p_rect.height(),
            getPlayStatePixmap()
        );
    }
}

void SpotifyTile::receiveExternalData(const QMimeData *data)
{
    Q_UNUSED(data);
    qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
    qDebug() << "  >" << "TODO: receive external data.";
    qDebug() << "  >" << data;
}


void SpotifyTile::receiveWheelEvent(QWheelEvent *event)
{
    // todo set to current volume
    int volume = 0;
    if (event->delta() < 0) {
        volume -= 3;
        if (volume < 0) {
            volume = 0;
        }
    }
    else if(event->delta() >= 0) {
        volume += 3;
        if (volume > 100){
            volume = 100;
        }
    }

//    settings->volume = volume;
//    if (pl->setSettings(settings)) {
//        emit wheelChangedVolume(volume);
//    }
}


const QJsonObject SpotifyTile::toJsonObject() const
{
    QJsonObject obj = BaseTile::toJsonObject();

    qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
    qDebug() << "  >" << "TODO: extend to json fucntion";


//    // store playlist
//    QJsonArray arr_pl;
//    foreach(DB::SoundFileRecord* rec, playlist_->getSoundFileList())
//        arr_pl.append(Misc::JsonMimeDataParser::toJsonObject(rec));
//    obj["playlist"] = arr_pl;

//    //store settings
//    QJsonObject obj_settings;
//    obj_settings = Misc::JsonMimeDataParser::toJsonObject(playlist_->getSettings());
//    obj["settings"] = obj_settings;

    return obj;
}

bool SpotifyTile::setFromJsonObject(const QJsonObject &obj)
{
    if(!BaseTile::setFromJsonObject(obj))
        return false;

    qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
    qDebug() << "  >" << "TODO: extend set from json";

    return true;
}

void SpotifyTile::play()
{
    if(is_playing_) {
        return;
    }

    SpotifyHandler::instance()->play(settings_);

    setIsPlaying(true);
}

void SpotifyTile::stop()
{
    if(!is_playing_) {
        return;
    }

    SpotifyHandler::instance()->stop();

    setIsPlaying(false);
}

void SpotifyTile::onActivate()
{
    if(is_playing_)
        stop();
    else
        play();

    BaseTile::onActivate();
}

void SpotifyTile::setVolume(int volume)
{
//    player_->setVolume(volume);
    Q_UNUSED(volume);
    qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
    qDebug() << "  >" << "TODO: set volume";
}

int SpotifyTile::getVolume() const
{
//    return player_->volume();

    qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
    qDebug() << "  >" << "TODO: get volume";

    return 0;
}


void SpotifyTile::onConfigure()
{
    SpotifyConfigureDialog d(settings_);

    if(d.exec() == SpotifyConfigureDialog::Rejected)
        return;

    settings_ = d.getSettings();
}

void SpotifyTile::onContents()
{
    QMessageBox b;
    b.setText("Configure show contents here");
    b.exec();
}

void SpotifyTile::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    if(mode_ != MOVE && e->button() == Qt::LeftButton) {
        if(is_playing_)
            stop();
        else
            play();
    }

    BaseTile::mouseReleaseEvent(e);
}


void SpotifyTile::createContextMenu()
{
    // create configure action
    QAction* configure_action = new QAction(tr("Configure..."),this);

    connect(configure_action, SIGNAL(triggered()),
            this, SLOT(onConfigure()));

    QAction* contents_action = new QAction(tr("Contents..."),this);

    connect(contents_action, SIGNAL(triggered()),
            this, SLOT(onContents()));

    context_menu_->addAction(configure_action);
    context_menu_->addAction(contents_action);
    context_menu_->addSeparator();

    BaseTile::createContextMenu();
}

const QPixmap SpotifyTile::getPlayStatePixmap() const
{
    if(is_playing_)
        return *Resources::Lib::PX_STOP;
    else
        return *Resources::Lib::PX_PLAY;
}

void SpotifyTile::setIsPlaying(bool state)
{
    is_activated_ = state;
    is_playing_ = state;
}

} // namespace Tile
