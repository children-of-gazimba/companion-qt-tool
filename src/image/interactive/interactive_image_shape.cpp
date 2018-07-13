#include "interactive_image_shape.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>

#include "resources/lib.h"

InteractiveImageShape::InteractiveImageShape(const QPainterPath& path, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , ActivationTracker()
    , path_(path)
    , is_uncover_shape_(true)
{
    setCleanByModelEnabled(false);
}

InteractiveImageShape::~InteractiveImageShape()
{
    if(scene())
        scene()->removeItem(this);
    if(Resources::Lib::TRACKER_MODEL->hasTracker(this))
        Resources::Lib::TRACKER_MODEL->removeTracker(this);
}

void InteractiveImageShape::setName(const QString &n)
{
    prepareGeometryChange();
    ActivationTracker::setName(n);
}

QRectF InteractiveImageShape::boundingRect() const
{
    return path_.boundingRect();
}

void InteractiveImageShape::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen p(painter->pen());
    p.setWidth(5);
    p.setColor(QColor(155,155,156));
    painter->setPen(p);
    painter->drawPath(path_);
    if(isSelected())
        painter->fillPath(path_, QColor(QColor(55,155,56)));
    else
        painter->fillPath(path_, QColor(QColor(55,55,56)));
    if(getName().size() > 0) {
        QFont f = painter->font();
        f.setPixelSize(30);
        painter->setFont(f);
        p.setColor(QColor(Qt::white));
        painter->drawText(boundingRect(), getName(), QTextOption(Qt::AlignLeft | Qt::AlignTop));
    }
}

QPainterPath InteractiveImageShape::shape() const
{
    return path_.simplified();
}

void InteractiveImageShape::setPath(const QPainterPath &p)
{
    prepareGeometryChange();
    path_ = p;
}

void InteractiveImageShape::setUncoverEnabled(bool enabled)
{
    is_uncover_shape_ = enabled;
}

bool InteractiveImageShape::getUncoverEnabled() const
{
    return is_uncover_shape_;
}
