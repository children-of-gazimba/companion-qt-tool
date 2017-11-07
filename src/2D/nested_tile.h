#ifndef TWO_D_NESTED_TILE_H
#define TWO_D_NESTED_TILE_H

#include <QGraphicsView>

#include "tile.h"
#include "graphics_view.h"
#include "db/model/sound_file_table_model.h"

namespace TwoD {

class NestedTile : public Tile
{
    Q_OBJECT

public:
    NestedTile(QGraphicsItem* parent = 0);
    virtual ~NestedTile();

    /**
     * Parses all tiles in scene to JSON object.
    */
    const QJsonObject toJsonObject() const;

    /**
     * tile from JSON object.
    */
    bool setFromJsonObject(const QJsonObject& obj);

    void setSoundFileModel(DB::Model::SoundFileTableModel* m);
    DB::Model::SoundFileTableModel* getSoundFileModel();

    /**
     * @brief see BC
     */
    QString const getClassName() const;

public slots:
    /* See BC */
    virtual void onActivate();

protected slots:
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

    GraphicsView* view_;
};

} // namespace TwoD

#endif // TWO_D_NESTED_TILE_H
