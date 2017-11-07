#include "nested_tile.h"

#include <QAction>
#include <QMenu>

namespace TwoD {

NestedTile::NestedTile(QGraphicsItem *parent)
    : Tile(parent)
    , view_(0)
{
    view_ = new GraphicsView;
    view_->setWindowFlags(Qt::Window);
    view_->hide();
}

NestedTile::~NestedTile()
{
    view_->deleteLater();
}

const QJsonObject NestedTile::toJsonObject() const
{
    QJsonObject obj = Tile::toJsonObject();

    obj["contents"] = view_->toJsonObject();

    return obj;
}

bool NestedTile::setFromJsonObject(const QJsonObject &obj)
{
    if(!Tile::setFromJsonObject(obj))
        return false;

    if(obj.contains("contents") && obj["contents"].isObject())
        view_->setFromJsonObject(obj["contents"].toObject());
    else
        return false;

    return true;
}

void NestedTile::setSoundFileModel(DB::Model::SoundFileTableModel *m)
{
    view_->setSoundFileModel(m);
}

DB::Model::SoundFileTableModel *NestedTile::getSoundFileModel()
{
    return view_->getSoundFileModel();
}

const QString NestedTile::getClassName() const
{
    return "NestedTile";
}

void NestedTile::onActivate()
{
    qDebug()  << "in activate of nested";
    foreach(QGraphicsItem* it, view_->scene()->items()) {
        QObject *obj = dynamic_cast<QObject*>(it);
        if(obj) {
            Tile* t = qobject_cast<Tile*>(obj);
            t->onActivate();
            qDebug() << "activate" << t->getName();
        }
    }

    Tile::onActivate();
}

void NestedTile::onContents()
{
    view_->show();
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
