#ifndef SPOTIFY_TILE_H
#define SPOTIFY_TILE_H

#include <QMediaPlayer>
#include <QGraphicsView>
#include <QMimeData>
#include <QDrag>

#include "base_tile.h"
#include "misc/json_mime_data_parser.h"
#include "spotify/spotify_remote_controller.h"

namespace Tile {

class SpotifyTile : public BaseTile
{
    Q_OBJECT

public:
    SpotifyTile(QGraphicsItem* parent = 0);
    virtual ~SpotifyTile();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual void receiveExternalData(const QMimeData* data);
    virtual void receiveWheelEvent(QWheelEvent *event);

    /**
     * Returns a QJsonObject holding all information about the tile
    */
    virtual const QJsonObject toJsonObject() const;

    /**
     * Set all values held by JSON object.
     * Returns success of parsing JsonObject.
    */
    virtual bool setFromJsonObject(const QJsonObject& obj);

    /**
     * @brief Returns the current settings object
     * @return
     */
    const SpotifyRemoteController::Settings &getSettings() const;

    /**
     * @brief creates Tile from WebLink
     * @param link
     */
    void fromWebLink(const QString &link);

    /**
     * @brief set Playing state
     * @param playing
     */
    void setPlaying(bool playing);

public slots:
    virtual void play();
    virtual void stop();
    virtual void onActivate();
    void setVolume(int volume);
    int getVolume() const;

signals:
    void wheelChangedVolume(int val);

protected slots:
    /** slot to open configuration */
    virtual void onConfigure();

    /** slot to open contents view */
    virtual void onContents();

protected:
    /*
     * BC overrides
    */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);

    /**
     * creates context menu
    */
    virtual void createContextMenu();

    /**
     * Returns the image based on the playback state
    */
    virtual const QPixmap getPlayStatePixmap() const;

    void setIsPlaying(bool state);

    bool is_playing_;
    QJsonDocument playback_info_;
    SpotifyRemoteController::Settings settings_;
};

} // namespace Tile

#endif // SPOTIFY_TILE_H

