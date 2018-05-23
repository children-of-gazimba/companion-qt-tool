#include "uncover_token_item.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QDebug>

UncoverTokenItem::UncoverTokenItem(QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , bounding_rect_(0, 0, 100, 100)
    , state_(IDLE)
    , uuid_(QUuid::createUuid())
    , click_offset_()
{
}

QRectF UncoverTokenItem::boundingRect() const
{
    return bounding_rect_;
}

void UncoverTokenItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if(state_ == IDLE)
        painter->setBrush(Qt::red);
    else if(state_ == SELECTED)
        painter->setBrush(Qt::blue);
    else if(state_ == MOVE)
        painter->setBrush(Qt::green);
    painter->drawEllipse(bounding_rect_);
}

const QUuid &UncoverTokenItem::getUuid() const
{
    return uuid_;
}

void UncoverTokenItem::mousePressEventAlt(QGraphicsSceneMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
        setState(MOVE);
        click_offset_ = e->pos() - pos();
    }
}

void UncoverTokenItem::mouseReleaseEventAlt(QGraphicsSceneMouseEvent *e)
{
    Q_UNUSED(e);
    setState(IDLE);
    qDebug().nospace() << Q_FUNC_INFO << " @ line " << __LINE__;
}

void UncoverTokenItem::mouseMoveEventAlt(QGraphicsSceneMouseEvent *e)
{
    if(state_ != MOVE)
        return;
    setPos(e->pos() - click_offset_);
    QPointF p_new = pos();
    if(parentItem() && !parentItem()->contains(p_new)) {
        qDebug().nospace() << Q_FUNC_INFO << " @ line " << __LINE__;
        qDebug() << "  > " << "moved out of view";
    }
    emit hasMoved();
    qDebug().nospace() << Q_FUNC_INFO << " @ line " << __LINE__;
}

void UncoverTokenItem::setState(UncoverTokenItem::State state)
{
    if(state != state_) {
        prepareGeometryChange();
        state_ = state;
    }
}
