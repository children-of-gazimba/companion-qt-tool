#include "nested_tile.h"
#include "playlist_player_tile.h"

#include <QAction>
#include <QMenu>
#include <QLayout>
#include <QJsonArray>

namespace TwoD {

NestedTile::NestedTile(GraphicsView* master_view, QGraphicsItem *parent)
    : Tile(parent)
    , master_view_(master_view)
    , scene_(0)
{
    scene_ = new QGraphicsScene(QRectF(0,0,100,100),this);
}

NestedTile::~NestedTile()
{
    clearTiles();
}

const QJsonObject NestedTile::toJsonObject() const
{
    QJsonObject obj = Tile::toJsonObject();

    QJsonObject contents_obj;

    // parse scene properties
    QJsonObject scene_obj;
    QJsonObject scene_rect_obj;
    scene_rect_obj["x"] = scene_->sceneRect().x();
    scene_rect_obj["y"] = scene_->sceneRect().y();
    scene_rect_obj["width"] = scene_->sceneRect().width();
    scene_rect_obj["height"] = scene_->sceneRect().height();
    scene_obj["scene_rect"] = scene_rect_obj;

    // parse all tiles in scene
    QJsonArray arr_tiles;
    foreach(QGraphicsItem* it, scene_->items()) {
        QObject *obj = dynamic_cast<QObject*>(it);
        if(obj) {
            Tile* t = qobject_cast<Tile*>(obj);
            QJsonObject obj_tile;
            obj_tile["type"] = QJsonValue(t->metaObject()->className());
            obj_tile["data"] = QJsonValue(t->toJsonObject());
            arr_tiles.append(obj_tile);
        }
    }
    scene_obj["tiles"] = QJsonValue(arr_tiles);

    contents_obj["scene"] = scene_obj;
    obj["contents"] = contents_obj;

    return obj;
}

bool NestedTile::setFromJsonObject(const QJsonObject &obj)
{
    if(!Tile::setFromJsonObject(obj))
        return false;

    if(!(obj.contains("contents") && obj["contents"].isObject()))
        return false;

    QJsonObject contents_obj = obj["contents"].toObject();
    if(contents_obj.isEmpty() || !contents_obj.contains("scene"))
        return false;
    if(!contents_obj["scene"].isObject())
        return false;

    QJsonObject sc_obj = contents_obj["scene"].toObject();
    if(!sc_obj.contains("scene_rect") || !sc_obj["scene_rect"].isObject())
        return false;
    if(!sc_obj.contains("tiles") || !sc_obj["tiles"].isArray())
        return false;

    // scene rect
    QJsonObject rc_obj = sc_obj["scene_rect"].toObject();
    qDebug() << rc_obj;
    if(rc_obj.contains("x") && rc_obj.contains("y") && rc_obj.contains("width") && rc_obj.contains("height")) {
        QRectF scene_rect = scene_->sceneRect();
        scene_rect.setX((qreal) rc_obj["x"].toDouble());
        scene_rect.setY((qreal) rc_obj["y"].toDouble());
        scene_rect.setWidth((qreal) rc_obj["width"].toDouble());
        scene_rect.setHeight((qreal) rc_obj["height"].toDouble());
        scene_->setSceneRect(scene_rect);
    }

    // clear tiles
    clearTiles();

    // tiles
    QJsonArray arr_tiles = sc_obj["tiles"].toArray();
    foreach(QJsonValue val, arr_tiles) {
        if(!val.isObject())
            continue;
        QJsonObject t_obj = val.toObject();
        if(!t_obj.contains("type") || !t_obj.contains("data") || !t_obj["data"].isObject())
            continue;

        // create tile, if type is TwoD::PlaylistPlayerTile
        if(t_obj["type"].toString().compare("TwoD::PlaylistPlayerTile") == 0) {
            PlaylistPlayerTile* tile = new PlaylistPlayerTile;
            tile->setSoundFileModel(master_view_->getSoundFileModel());
            tile->setFlag(QGraphicsItem::ItemIsMovable, true);
            tile->init();
            if(tile->setFromJsonObject(t_obj["data"].toObject())) {
                scene_->addItem(tile);
            }
            else {
                qDebug() << "FAILURE: Could not set Tile data from JSON.";
                qDebug() << " > data:" << t_obj["data"];
                qDebug() << " > Aborting.";
                delete tile;
                return false;
            }
        }
        else if(t_obj["type"].toString().compare("TwoD::NestedTile") == 0) {
            NestedTile* tile = new NestedTile(master_view_);
            tile->setFlag(QGraphicsItem::ItemIsMovable, true);
            tile->init();
            if(tile->setFromJsonObject(t_obj["data"].toObject())) {
                scene_->addItem(tile);
            }
            else {
                qDebug() << "FAILURE: Could not set Tile data from JSON.";
                qDebug() << " > data:" << t_obj["data"];
                qDebug() << " > Aborting.";
                delete tile;
                return false;
            }
        }
    }

    return true;
}

const QString NestedTile::getClassName() const
{
    return "NestedTile";
}

void NestedTile::clearTiles()
{
    foreach(QGraphicsItem* it, scene_->items()) {
        QObject* o = dynamic_cast<QObject*>(it);
        if(o) {
            Tile* t = qobject_cast<Tile*>(o);
            t->onDelete();
        }
    }
}

void NestedTile::onActivate()
{
    foreach(QGraphicsItem* it, scene_->items()) {
        QObject *obj = dynamic_cast<QObject*>(it);
        if(obj) {
            Tile* t = qobject_cast<Tile*>(obj);
            t->onActivate();
        }
    }

    Tile::onActivate();
}

void NestedTile::onDelete()
{
    clearTiles();
    Tile::onDelete();
}

void NestedTile::onContents()
{
    master_view_->pushScene(scene_);
}

void NestedTile::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    if(mode_ != MOVE && e->button() == Qt::LeftButton)
        onActivate();

    Tile::mouseReleaseEvent(e);
}

void NestedTile::createContextMenu()
{
    QAction* contents_action = new QAction(tr("Contents..."),this);

    connect(contents_action, SIGNAL(triggered()),
            this, SLOT(onContents()));

    context_menu_->addAction(contents_action);
    context_menu_->addSeparator();

    Tile::createContextMenu();
}

}
