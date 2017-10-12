#ifndef TWO_D_GRAPHICS_VIEW_H
#define TWO_D_GRAPHICS_VIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QJsonObject>
#include <QUuid>

#include "db/model/sound_file_table_model.h"

// TODO: rename namespace to Tile
namespace TwoD {

/**
 * Custom QGraphicsView.
 * Designed for advanced handling of Tile (see Tile.h).
 * Evaulates drops, instanciating derived Tile objects.
 * Implements bahavior for adapting screen size to widget resize.
 * Implements forwarding of drops to colliding Tile instances.
 * Holds functionality to convert all tiles in scene to JSON description
 * and be set from JSON.
*/
class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    GraphicsView(QGraphicsScene *scene, QWidget *parent);
    GraphicsView(QWidget *parent);

    /**
     * Parses all tiles in scene to JSON object.
    */
    const QJsonObject toJsonObject() const;

    /**
     * Creates all tiles in scene from JSON object.
     * Deletes existing scene.
     * Returns success of parsing.
    */
    bool setFromJsonObject(const QJsonObject& obj);

    void setSoundFileModel(DB::Model::SoundFileTableModel* m);
    DB::Model::SoundFileTableModel* getSoundFileModel();

    /**
     * Activates tile with given ID.
     * Returns true if activation succeeded.
    */
    bool activate(const QUuid& tile_id);

    /**
     * Deactivates tile with given ID.
     * Returns true if deactivation succeeded.
    */
    bool deactivate(const QUuid& tile_id);

    /**
     * Returns true if tile with given ID
     * is currently activated.
    */
    bool isActivated(const QUuid &tile_id);

    /**
     * @brief returns volume of tile or -1 if tile not found
     * @param tile_id: id of tile to receive volume of.
     * @return volume of tile.
     */
    int getVolume(const QUuid &tile_id) const;

    /**
     * @brief set volume of tile (range 0-100)
     * @param tile_id: id of tile to receive volume of.
     * @return success of setting volume.
     */
    bool setVolume(const QUuid& tile_id, int volume);

private:
    /**
     * Handle scene size when widget resizes.
     * Scene extends with bigger widget size.
     * Scene never shrinks.
     * (so items placed in scene bounds can not get lost without moving).
    */
    void resizeEvent(QResizeEvent* e);

signals:
    void dropAccepted();

private:
    /**
     * accept drags.
    */
    void dragEnterEvent(QDragEnterEvent *event);

    /**
     * accept move.
    */
    void dragMoveEvent(QDragMoveEvent *event);

    /**
    * Derived tile types get deleted on drop into empty area of scene.
    * Drops are forwarded to Tile containing mouse position.
    */
    void dropEvent(QDropEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    /**
     * Remove all tiles from view.
     */
    void clearTiles();

    DB::Model::SoundFileTableModel* model_;
};

}

#endif // TWO_D_GRAPHICS_VIEW_H
