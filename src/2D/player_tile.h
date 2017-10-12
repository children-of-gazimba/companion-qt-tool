#ifndef TWO_D_PLAYER_TILE_H
#define TWO_D_PLAYER_TILE_H

#include "tile.h"

#include <QMediaPlayer>

namespace TwoD {

/**
* Tile encapsulating simple media playback features and UI.
*/
class PlayerTile : public Tile
{
    Q_OBJECT

public:
    PlayerTile(QGraphicsItem* parent = 0);
    virtual ~PlayerTile();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    /**
     * @brief see BC
     */
    QString const getClassName() const;
public slots:
    virtual void setMedia(const QMediaContent& c);
    virtual void play();
    virtual void stop();

protected:
    /*
     * BC overrides
    */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);

    /**
     * Returns QPixmap image based on the playback state.
    */
    virtual const QPixmap getPlayStatePixmap() const;

    void setIsPlaying(bool);

    QMediaPlayer* player_;
    bool is_playing_;
};

} // namespace TwoD

#endif // TWO_D_PLAYER_TILE_H
