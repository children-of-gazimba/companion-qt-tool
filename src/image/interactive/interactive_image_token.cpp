#include "interactive_image_token.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QDebug>

#include "resources/lib.h"

InteractiveImageToken::InteractiveImageToken(QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , bounding_rect_(0, 0, 100, 100)
    , state_(IDLE)
    , uuid_(QUuid::createUuid())
    , uncover_radius_(0.0f)
{
    uncover_radius_ = sqrt(100*100 + 100*100);
    setFlag(QGraphicsItem::ItemIsMovable, true);
}

InteractiveImageToken::InteractiveImageToken(const QSizeF &s, QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , bounding_rect_(0, 0, s.width(), s.height())
    , state_(IDLE)
    , uuid_(QUuid::createUuid())
    , uncover_radius_(0.0f)
{
    uncover_radius_ = sqrt(s.width()*s.width() + s.height()*s.height());
    setFlag(QGraphicsItem::ItemIsMovable, true);
}

QRectF InteractiveImageToken::boundingRect() const
{
    return bounding_rect_;
}

void InteractiveImageToken::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    setOpacity(0.3f);
    if(state_ == IDLE) {
        painter->setBrush(Qt::red);
    }
    else if(state_ == SELECTED) {
        setOpacity(0.6f);
        painter->setBrush(Qt::blue);
    }
    else if(state_ == MOVE) {
        setOpacity(0.9f);
        painter->setBrush(Qt::green);
    }
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(bounding_rect_);
    setOpacity(0.6f);
    painter->drawPixmap(bounding_rect_.toRect(), *Resources::Lib::PX_COMPANION);
}

const QRectF InteractiveImageToken::uncoverBoundingRect() const
{
    return QRectF(0, 0, uncover_radius_, uncover_radius_);
}

const QUuid &InteractiveImageToken::getUuid() const
{
    return uuid_;
}

float InteractiveImageToken::getUncoverRadius() const
{
    return uncover_radius_;
}

void InteractiveImageToken::setUncoverRadius(float r)
{
    uncover_radius_ = r;
}

void InteractiveImageToken::setSize(const QSizeF &s)
{
    prepareGeometryChange();
    bounding_rect_.setSize(s);
}

const QPointF InteractiveImageToken::centerPos() const
{
    return mapToScene(boundingRect().center());
}

void InteractiveImageToken::setState(InteractiveImageToken::State state)
{
    if(state != state_) {
        prepareGeometryChange();
        state_ = state;
    }
}

void InteractiveImageToken::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
        setState(MOVE);
    }
    else if(e->button() == Qt::RightButton) {
        // qDebug() << "right button";
    }
    else if(e->button() == Qt::MidButton) {
        // qDebug() << "mid button";
    }

    QGraphicsObject::mousePressEvent(e);
}

void InteractiveImageToken::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    setState(IDLE);
    QGraphicsObject::mouseReleaseEvent(e);
}

void InteractiveImageToken::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    if(state_ == MOVE) {
        QPointF p = pos();
        QGraphicsItem::mouseMoveEvent(e);
        QPointF p_new = pos();
        Q_UNUSED(p);
        Q_UNUSED(p_new);
        emit hasMoved();
    }
}