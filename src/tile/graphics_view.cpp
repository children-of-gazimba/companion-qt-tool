#include "graphics_view.h"

#include <QDebug>
#include <QJsonArray>
#include <QMimeData>

#include "playlist_tile.h"
#include "nested_tile.h"
#include "misc/json_mime_data_parser.h"

namespace Tile {

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
    , sound_model_(0)
    , main_scene_(scene)
    , scene_stack_()
    , context_menu_(0)
    , click_pos_()
{
    pushScene(main_scene_);
    setAcceptDrops(true);
    setFocusPolicy(Qt::ClickFocus);
    initContextMenu();
}

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent)
    , sound_model_(0)
    , main_scene_(0)
    , scene_stack_()
    , context_menu_(0)
    , click_pos_()
{
    main_scene_ = new QGraphicsScene(QRectF(0,0,100,100),this);
    pushScene(main_scene_);
    setAcceptDrops(true);
    setFocusPolicy(Qt::ClickFocus);
    initContextMenu();
}

GraphicsView::~GraphicsView()
{
    context_menu_->deleteLater();
}

const QJsonObject GraphicsView::toJsonObject() const
{
    QJsonObject obj;

    // parse scene properties
    QJsonObject scene_obj;
    QJsonObject scene_rect_obj;
    scene_rect_obj["x"] = sceneRect().x();
    scene_rect_obj["y"] = sceneRect().y();
    scene_rect_obj["width"] = sceneRect().width();
    scene_rect_obj["height"] = sceneRect().height();
    scene_obj["scene_rect"] = scene_rect_obj;

    // parse all tiles in scene
    QJsonArray arr_tiles;
    foreach(QGraphicsItem* it, scene()->items()) {
        QObject *obj = dynamic_cast<QObject*>(it);
        if(obj) {
            BaseTile* t = qobject_cast<BaseTile*>(obj);
            QJsonObject obj_tile;
            obj_tile["type"] = QJsonValue(t->metaObject()->className());
            obj_tile["data"] = QJsonValue(t->toJsonObject());
            arr_tiles.append(obj_tile);
        }
    }
    scene_obj["tiles"] = QJsonValue(arr_tiles);

    obj["scene"] = scene_obj;

    return obj;
}

bool GraphicsView::setFromJsonObject(const QJsonObject &obj)
{
    if(obj.isEmpty() || !obj.contains("scene"))
        return false;
    if(!obj["scene"].isObject())
        return false;

    QJsonObject sc_obj = obj["scene"].toObject();
    if(!sc_obj.contains("scene_rect") || !sc_obj["scene_rect"].isObject())
        return false;
    if(!sc_obj.contains("tiles") || !sc_obj["tiles"].isArray())
        return false;

    // scene rect
    QJsonObject rc_obj = sc_obj["scene_rect"].toObject();
    if(rc_obj.contains("x") && rc_obj.contains("y") && rc_obj.contains("width") && rc_obj.contains("height")) {
        QRectF scene_rect = sceneRect();
        scene_rect.setX((qreal) rc_obj["x"].toDouble());
        scene_rect.setY((qreal) rc_obj["y"].toDouble());
        scene_rect.setWidth((qreal) rc_obj["width"].toDouble());
        scene_rect.setHeight((qreal) rc_obj["height"].toDouble());
        //setSceneRect(scene_rect);
        scene()->setSceneRect(scene_rect);
        //qDebug() << scene()->sceneRect();
    }

    clearTiles();

    QString pl_class = PlaylistTile::staticMetaObject.className();
    QString nested_class = NestedTile::staticMetaObject.className();

    // tiles
    QJsonArray arr_tiles = sc_obj["tiles"].toArray();
    foreach(QJsonValue val, arr_tiles) {
        if(!val.isObject())
            continue;
        QJsonObject t_obj = val.toObject();
        if(!t_obj.contains("type") || !t_obj.contains("data") || !t_obj["data"].isObject())
            continue;

        // create tile, if type is Tile::PlaylistTile
        if(t_obj["type"].toString().compare(pl_class) == 0) {
            PlaylistTile* tile = new PlaylistTile;
            tile->setSoundFileModel(sound_model_);
            tile->setPresetModel(preset_model_);
            tile->setFlag(QGraphicsItem::ItemIsMovable, true);
            tile->init();
            if(tile->setFromJsonObject(t_obj["data"].toObject())) {
                scene()->addItem(tile);
            }
            else {
                qDebug() << "FAILURE: Could not set Tile data from JSON.";
                qDebug() << " > data:" << t_obj["data"];
                qDebug() << " > Aborting.";
                delete tile;
                return false;
            }
        }
        else if(t_obj["type"].toString().compare(nested_class) == 0) {
            NestedTile* tile = new NestedTile(this);
            tile->setPresetModel(preset_model_);
            tile->setFlag(QGraphicsItem::ItemIsMovable, true);
            tile->init();
            if(tile->setFromJsonObject(t_obj["data"].toObject())) {
                scene()->addItem(tile);
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

void GraphicsView::setSoundFileModel(DB::Model::SoundFileTableModel *m)
{
    sound_model_ = m;
}

DB::Model::SoundFileTableModel *GraphicsView::getSoundFileModel()
{
    return sound_model_;
}

void GraphicsView::setPresetModel(DB::Model::PresetTableModel *m)
{
    preset_model_ = m;
}

DB::Model::PresetTableModel *GraphicsView::getPresetModel()
{
    return preset_model_;
}

bool GraphicsView::activate(const QUuid &tile_id)
{
    foreach(QGraphicsItem* it, scene()->items()) {
        QObject* o = dynamic_cast<QObject*>(it);
        if(o) {
            BaseTile* t = qobject_cast<BaseTile*>(o);
            if(t && t->getUuid() == tile_id && !t->isActivated()) {
                t->onActivate();
                return true;
            }
        }
    }
    return false;
}

bool GraphicsView::deactivate(const QUuid &tile_id)
{
    foreach(QGraphicsItem* it, scene()->items()) {
        QObject* o = dynamic_cast<QObject*>(it);
        if(o) {
            BaseTile* t = qobject_cast<BaseTile*>(o);
            if(t && t->getUuid() == tile_id && t->isActivated()) {
                t->onActivate();
                return true;
            }
        }
    }
    return false;
}

bool GraphicsView::isActivated(const QUuid &tile_id)
{
    foreach(QGraphicsItem* it, scene()->items()) {
        QObject* o = dynamic_cast<QObject*>(it);
        if(o) {
            BaseTile* t = qobject_cast<BaseTile*>(o);
            if(t && t->getUuid() == tile_id && t->isActivated()) {
                return t->isActivated();
            }
        }
    }
    return false;
}

int GraphicsView::getVolume(const QUuid &tile_id) const
{
    foreach(QGraphicsItem* it, scene()->items()) {
        QObject* o = dynamic_cast<QObject*>(it);
        if(o) {
            BaseTile* t = qobject_cast<BaseTile*>(o);
            if(t && t->getUuid() == tile_id && t->isActivated()) {
                if(t->getClassName().compare("PlaylistTile") == 0) {
                    PlaylistTile* p = (PlaylistTile*) t;
                    return p->getVolume();
                }
            }
        }
    }
    return -1;
}

bool GraphicsView::setVolume(const QUuid &tile_id, int volume)
{
    foreach(QGraphicsItem* it, scene()->items()) {
        QObject* o = dynamic_cast<QObject*>(it);
        if(o) {
            BaseTile* t = qobject_cast<BaseTile*>(o);
            if(t && t->getUuid() == tile_id && t->isActivated()) {
                if(t->getClassName().compare("PlaylistTile") == 0) {
                    PlaylistTile* p = (PlaylistTile*) t;
                    p->setVolume(volume);
                    return true;
                }
            }
        }
    }
    return false;
}

void GraphicsView::pushScene(QGraphicsScene* scene)
{
    scene_stack_.push(scene);
    setScene(scene);
}

void GraphicsView::popScene()
{
    if(scene_stack_.size() > 1) {
        scene_stack_.pop();
        setScene(scene_stack_.top());
    }
}

const QMenu *GraphicsView::getContextMenu() const
{
    return context_menu_;
}

void GraphicsView::createEmptyPlaylistTile(const QPoint &p)
{
    PlaylistTile* tile = new PlaylistTile;
    tile->setPresetModel(preset_model_);
    tile->setSoundFileModel(sound_model_);
    tile->setFlag(QGraphicsItem::ItemIsMovable, true);
    tile->init();
    tile->setPos(p);
    tile->setSize(0);
    tile->setName("Empty Playlist");

    // add to scene
    scene()->addItem(tile);
    tile->setSmallSize();
}

void GraphicsView::createEmptyNestedTile(const QPoint &p)
{
    NestedTile* tile = new NestedTile(this);
    tile->setPresetModel(preset_model_);
    tile->setFlag(QGraphicsItem::ItemIsMovable, true);
    tile->init();
    tile->setPos(p);
    tile->setSize(0);
    tile->setName("Empty Nested");

    // add to scene
    scene()->addItem(tile);
    tile->setSmallSize();
}

void GraphicsView::resizeEvent(QResizeEvent *e)
{
    QGraphicsView::resizeEvent(e);
    if(e->isAccepted()) {
        QRectF r = scene()->sceneRect();
        if(e->size().width() > r.width()) {
            r.setWidth(e->size().width());
        }
        if(e->size().height() > r.height()) {
            r.setHeight(e->size().height());
        }
        scene()->setSceneRect(r);
        //setSceneRect(r);
    }
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    if(!scene())
        return;

    QPointF p(mapToScene(event->pos()));

    foreach(QGraphicsItem* item, scene()->items()){
        if (item->contains(item->mapFromScene(p))){
            QObject *selected_object = dynamic_cast<QObject*>(item);
            if(selected_object)
            {
                BaseTile* t = qobject_cast<BaseTile*>(selected_object);
                if(t){
                    t->receiveWheelEvent(event);
                    return;
                }
            }
        }
    }
    QGraphicsView::wheelEvent(event);
}

void GraphicsView::onEmptyPlaylistTile()
{
    createEmptyPlaylistTile(click_pos_);
}

void GraphicsView::onEmptyNestedTile()
{
    createEmptyNestedTile(click_pos_);
}

void GraphicsView::dragEnterEvent(QDragEnterEvent *event)
{
    //qDebug() << "GraphicView: drag Enter Event ";
    GraphicsView *source = qobject_cast<GraphicsView*>(event->source());
    if (event->source() && source != this) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void GraphicsView::dragMoveEvent(QDragMoveEvent *event)
{
    //qDebug() << "GraphicView: drag Enter Move";
    GraphicsView *source = qobject_cast<GraphicsView*>(event->source());
    if (event->source() && source != this) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void GraphicsView::dropEvent(QDropEvent *event)
{
    if(!scene())
        return;

    QPointF p(mapToScene(event->pos()));

    foreach(QGraphicsItem* item, scene()->items()){
        if (item->contains(item->mapFromScene(p))){
            QObject *selected_object = dynamic_cast<QObject*>(item);
            if(selected_object)
            {
                BaseTile* t = qobject_cast<BaseTile*>(selected_object);
                if(t){
                    t->receiveExternalData(event->mimeData());
                    event->accept();
                    return;
                }
            }
        }
    }

    // extract DB::TableRecord from mime data
    QList<DB::TableRecord*> records = Misc::JsonMimeDataParser::toTableRecordList(event->mimeData());

    QString pl_class = PlaylistTile::staticMetaObject.className();
    QString nested_class = NestedTile::staticMetaObject.className();

    // validate parsing
    if(records.size() == 0 || records[0]->index != DB::SOUND_FILE) {
        // TODO make pretty
        QJsonDocument doc = QJsonDocument::fromJson(event->mimeData()->text().toUtf8());
        if(doc.object().contains("type") && doc.object()["type"].toString().compare(nested_class) == 0) {
            NestedTile* tile = new NestedTile(this);
            tile->setPresetModel(preset_model_);
            tile->setFlag(QGraphicsItem::ItemIsMovable, true);
            tile->setFromJsonObject(doc.object()["data"].toObject());
            tile->init();
            tile->setPos(p);
            tile->setSize(0);

            // add to scene
            scene()->addItem(tile);
            tile->setSmallSize();

            // except event
            event->setDropAction(Qt::CopyAction);
            event->accept();
            emit dropAccepted();
            return;
        }
        else if(doc.object().contains("type") && doc.object()["type"].toString().compare(pl_class) == 0) {
            qDebug() << "received";
            PlaylistTile* tile = new PlaylistTile;
            tile->setPresetModel(preset_model_);
            tile->setSoundFileModel(sound_model_);
            tile->setFlag(QGraphicsItem::ItemIsMovable, true);
            tile->setFromJsonObject(doc.object()["data"].toObject());
            tile->init();
            tile->setPos(p);
            tile->setSize(0);

            // add to scene
            scene()->addItem(tile);
            tile->setSmallSize();

            // except event
            event->setDropAction(Qt::CopyAction);
            event->accept();
            emit dropAccepted();
            return;
        }
        else {
            event->ignore();
            return;
        }
    }

    // create graphics item
    PlaylistTile* tile = new PlaylistTile;
    tile->setSoundFileModel(sound_model_);
    tile->setPresetModel(preset_model_);
    tile->setFlag(QGraphicsItem::ItemIsMovable, true);
    tile->setName(records[0]->name);
    tile->init();
    tile->setPos(p);
    tile->setSize(0);

    foreach(DB::TableRecord* rec, records) {
        if(rec->index == DB::SOUND_FILE)
            tile->addMedia(*((DB::SoundFileRecord*) rec));
    }

    // add to scene
    scene()->addItem(tile);
    tile->setSmallSize();

    // except event
    event->setDropAction(Qt::CopyAction);
    event->accept();
    emit dropAccepted();

    // delete temp records
    while(records.size() > 0) {
        delete records[0];
        records.pop_front();
    }
}

void GraphicsView::keyPressEvent(QKeyEvent*)
{
    //qDebug() << event->key();
}

void GraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Backspace) {
        popScene();
        return;
    }

    foreach(QGraphicsItem* it, scene()->items()) {
        QObject* o = dynamic_cast<QObject*>(it);
        if(o) {
            BaseTile* t = qobject_cast<BaseTile*>(o);
            if(t->hasActivateKey() && t->getActivateKey() == QChar(event->key()))
                t->onActivate();
        }
    }
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    if(!event->isAccepted()) {
        if(event->button() == Qt::RightButton) {
            context_menu_->popup(event->globalPos());
            click_pos_ = event->pos();
            event->accept();
        }
    }
}

void GraphicsView::clearTiles()
{
    foreach(QGraphicsItem* it, scene()->items()) {
        QObject* o = dynamic_cast<QObject*>(it);
        if(o) {
            BaseTile* t = qobject_cast<BaseTile*>(o);
            t->onDelete();
        }
    }
}

void GraphicsView::initContextMenu()
{
    context_menu_ = new QMenu;

    QMenu* create_empty = new QMenu(tr("Create Empty"));

    QAction* empty_playlist = new QAction(tr("Playlist Tile"));
    connect(empty_playlist, SIGNAL(triggered()),
            this, SLOT(onEmptyPlaylistTile()));

    QAction* empty_nested = new QAction(tr("Nested Tile"));
    connect(empty_nested, SIGNAL(triggered()),
            this, SLOT(onEmptyNestedTile()));

    create_empty->addAction(empty_playlist);
    create_empty->addAction(empty_nested);

    context_menu_->addMenu(create_empty);
}

} // namespace Tile
