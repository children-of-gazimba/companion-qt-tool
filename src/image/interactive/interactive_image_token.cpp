#include "interactive_image_token.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QDebug>
#include <cmath>
#include <QGraphicsScene>

#include "resources/lib.h"
#include "tracking/tracker.h"

InteractiveImageToken::InteractiveImageToken(QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , Trackable()
    , bounding_rect_(-50, -50, 100, 100)
    , state_(IDLE)
    , uuid_(QUuid::createUuid())
    , uncover_radius_(0.0f)
    , name_("")
{
    uncover_radius_ = sqrt(100*100 + 100*100);
    setFlag(QGraphicsItem::ItemIsMovable, true);
}

InteractiveImageToken::InteractiveImageToken(const QSizeF &s, QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , Trackable()
    , bounding_rect_(-s.width()/2.0, -s.height()/2.0, s.width(), s.height())
    , state_(IDLE)
    , uuid_(QUuid::createUuid())
    , uncover_radius_(0.0f)
    , name_("")
{
    uncover_radius_ = sqrt(s.width()*s.width() + s.height()*s.height());
    setFlag(QGraphicsItem::ItemIsMovable, true);
}

InteractiveImageToken::InteractiveImageToken(const InteractiveImageToken &it, QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , bounding_rect_(it.boundingRect())
    , state_(IDLE)
    , uuid_(it.getUuid())
    , uncover_radius_(it.getUncoverRadius())
{
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
//    setOpacity(0.6f);
//    painter->drawPixmap(bounding_rect_.toRect(), *Resources::Lib::PX_COMPANION);
}

bool InteractiveImageToken::updateLinkFromTracker(Tracker *tracker, int target_prop)
{
    if(!Trackable::updateLinkFromTracker(tracker, target_prop))
        return false;

    if(!scene())
        return false;

    QPointF uncover_pos;
    switch(target_prop) {
        case Tracker::ALL:
            uncover_pos.setX(tracker->getRelativePosition().x() * scene()->width());
            uncover_pos.setY(tracker->getRelativePosition().y() * scene()->height());
            setRotation(tracker->getRotation());
            break;
        case Tracker::POSITION:
            setUncoverPos(tracker->getPosition());
            break;
        case Tracker::REL_POSITION:
            uncover_pos.setX(tracker->getRelativePosition().x() * scene()->width());
            uncover_pos.setY(tracker->getRelativePosition().y() * scene()->height());
            break;
        case Tracker::ROTATION:
            setRotation(tracker->getRotation());
            break;
        default:break;
    }

    if(!uncover_pos.isNull())
        setUncoverPos(uncover_pos);

    return true;
}

bool InteractiveImageToken::updateGrabFromTracker(Tracker *tracker, int target_prop)
{
    if(!Trackable::updateGrabFromTracker(tracker, target_prop))
        return false;
    qDebug().nospace() << Q_FUNC_INFO << " @ line " << __LINE__;
    qDebug() << "  > " << "TODO implement grab";
    return true;
}

const QRectF InteractiveImageToken::uncoverBoundingRect() const
{
    return QRectF(-uncover_radius_/2.0, -uncover_radius_/2.0, uncover_radius_, uncover_radius_);
}

const QString &InteractiveImageToken::getName() const
{
    return name_;
}

void InteractiveImageToken::setName(const QString &n)
{
    name_ = n;
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

void InteractiveImageToken::setUncoverPos(const QPointF &pos)
{
    setPos(pos);
    emit hasMoved();
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
