#ifndef TWO_D_NESTED_TILE_H
#define TWO_D_NESTED_TILE_H

#include <QGraphicsView>

#include "base_tile.h"
#include "graphics_view.h"
#include "db/model/sound_file_table_model.h"

namespace Tile {

class NestedTile : public BaseTile
{
    Q_OBJECT

public:
    NestedTile(GraphicsView* master_view, QGraphicsItem *parent=0);
    virtual ~NestedTile();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    /**
     * Parses all tiles in scene to JSON object.
    */
    const QJsonObject toJsonObject() const;

    /**
     * tile from JSON object.
    */
    bool setFromJsonObject(const QJsonObject& obj);

    void clearTiles();

    void addTiles(QList<BaseTile*> const& tiles);

public slots:
    /* See BC */
    virtual void onActivate();

protected slots:
    /** slot to open contents view */
    virtual void onContents();

    /** slot to open configuration */
    virtual void onConfigure();

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

    GraphicsView* master_view_;
    QGraphicsScene* scene_;
};

} // namespace Tile

#endif // TWO_D_NESTED_TILE_H
