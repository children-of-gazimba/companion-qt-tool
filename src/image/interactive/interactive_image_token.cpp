#include "interactive_image_token.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QDebug>
#include <cmath>
#include <QGraphicsScene>
#include <QVector2D>
#include <QPen>
#include <QPainterPath>

#include "resources/lib.h"
#include "tracking/tracker.h"

#define TEXT_HEIGHT 50.0f
#define TEXT_WIDTH 500.0f

InteractiveImageToken::InteractiveImageToken(QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , Trackable()
    , marker_rect_(-50, -50, 100, 100)
    , grab_rect_(-50, -50, 100, 100)
    , uncover_rect_()
    , state_(IDLE)
    , uuid_(QUuid::createUuid())
    , uncover_radius_(0.0f)
    , uncover_indicator_radius(0.0f)
    , name_("")
    , color_(55,55,56)
    , show_grab_indicator_(false)
    , grab_radius_(0.0f)
    , grabbed_rotation_(0.0f)
    , token_grabbed_(false)
    , grabbed_position_()
    , grabbed_relative_position_()
{
    uncover_radius_ = sqrt(300*300 + 300*300);
    uncover_indicator_radius = uncover_radius_;
    grab_radius_ = sqrt(300*300 + 300*300);
    uncover_rect_ = QRectF(-uncover_radius_, -uncover_radius_, uncover_radius_ * 2, uncover_radius_ * 2);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setZValue(1);
}

InteractiveImageToken::InteractiveImageToken(const QSizeF &s, QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , Trackable()
    , marker_rect_(-s.width()/2.0, -s.height()/2.0, s.width(), s.height())
    , grab_rect_(-s.width()/2.0, -s.height()/2.0, s.width(), s.height())
    , uncover_rect_()
    , state_(IDLE)
    , uuid_(QUuid::createUuid())
    , uncover_radius_(0.0f)
    , uncover_indicator_radius(0.0f)
    , name_("")
    , color_(55,55,56)
    , show_grab_indicator_(false)
    , grabbed_rotation_(0.0f)
    , token_grabbed_(false)
    , grabbed_position_()
    , grabbed_relative_position_()
{
    uncover_radius_ = sqrt(s.width()*s.width() + s.height()*s.height());
    uncover_indicator_radius = uncover_radius_;
    uncover_rect_ = QRectF(-uncover_radius_, -uncover_radius_, uncover_radius_ * 2, uncover_radius_ * 2);
    grab_radius_ = sqrt(s.width()*s.width() + s.height()*s.height());

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setZValue(1);
}

InteractiveImageToken::InteractiveImageToken(const InteractiveImageToken &it, QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , marker_rect_(it.boundingRect())
    , state_(IDLE)
    , uuid_(it.getUuid())
    , uncover_radius_(it.getUncoverRadius())
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
}

InteractiveImageToken::~InteractiveImageToken()
{
    if(scene())
        scene()->removeItem(this);
}

QRectF InteractiveImageToken::boundingRect() const
{
    return calculateBoundingRect();
}

QRectF InteractiveImageToken::markerRect() const
{
    return marker_rect_;
}

QRectF InteractiveImageToken::textRect() const
{
    return QRectF(
                -TEXT_WIDTH/2.0f,
                marker_rect_.bottom(),
                TEXT_WIDTH,
                TEXT_HEIGHT
                );
}

QRectF InteractiveImageToken::grabRect() const
{
    return grab_rect_;
}

void InteractiveImageToken::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    //painter->setRenderHint(QPainter::SmoothPixmapTransform);

    setOpacity(0.8f);
    if(state_ == IDLE) {
        //painter->setBrush(Qt::red);
    }
    else if(state_ == SELECTED) {
        setOpacity(1.0f);
        //painter->setBrush(Qt::blue);
    }
    else if(state_ == MOVE) {
        setOpacity(1.0f);
        //painter->setBrush(Qt::green);
    }
    QPen p(Qt::white);
    p.setWidth(4);
    painter->setPen(p);
    painter->setBrush(color_);
    painter->drawEllipse(markerRect());
    if(show_grab_indicator_) {
        p.setWidth(3);
        p.setColor(Qt::white);
        painter->setPen(p);
        painter->setOpacity(0.3);
        painter->drawEllipse(markerRect().center(), grab_radius_, grab_radius_);
    }
    if(show_uncover_indicator_) {
        p.setWidth(3);
        p.setColor(Qt::red);
        painter->setPen(p);
        painter->setOpacity(0.3);
        painter->drawEllipse(markerRect().center(), uncover_indicator_radius, uncover_indicator_radius);
    }
    QFont f = painter->font();
    f.setPixelSize(TEXT_HEIGHT);
    f.setWeight((int)(f.weight()*1.5f));
    p.setColor(QColor(Qt::white));
    painter->setPen(p);
    painter->setFont(f);
    painter->drawText(textRect(), getName(), QTextOption(Qt::AlignCenter));
    /*setOpacity(0.6f);
    painter->drawPixmap(bounding_rect_.toRect(), *Resources::Lib::PX_COMPANION);*/
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
    if(!ensureGrabbable(tracker, target_prop))
        return false;

    QVector2D tracker_pos(tracker->getRelativePosition().x() * scene()->width(),
                          tracker->getRelativePosition().y() * scene()->height());
    float distance_to_tracker = (tracker_pos - QVector2D(pos())).length();

    if(distance_to_tracker <= grab_radius_) {
        token_grabbed_ = true;
    }

    QPointF uncover_diff(
            (tracker->getRelativePosition().x() - grabbed_relative_position_.x()) * scene()->width(),
            (tracker->getRelativePosition().y() - grabbed_relative_position_.y()) * scene()->height()
            );


    QPointF uncover_pos( pos().x(), pos().y() );
    if(token_grabbed_)
        uncover_pos = QPointF(
                    pos().x() + uncover_diff.x(),
                    pos().y() + uncover_diff.y()
                    );

    float new_rotation = tracker->getRotation() - grabbed_rotation_;

    grabbed_relative_position_ = tracker->getRelativePosition();

    switch(target_prop) {
        case Tracker::ALL:
            setUncoverPos(uncover_pos);
            setRotation(new_rotation);
            break;
        case Tracker::REL_POSITION:
            setUncoverPos(uncover_pos);
            break;
        case Tracker::ROTATION:
            setRotation(new_rotation);
            break;
        default:break;
    }

    return true;
}

bool InteractiveImageToken::registerGrab(Tracker *tracker, int target_prop)
{
    if(hasLink(target_prop)) {
        removeLink(target_prop);
    } else if (hasGrab(target_prop)) {
        removeGrab(target_prop);
    }

    if(!prepareRegistration(target_prop))
        return false;

    switch(target_prop) {
        case Tracker::ROTATION:
            grabbed_rotation_ = rotation();
            break;
        case Tracker::POSITION:
            grabbed_position_ = pos();
            break;
        case Tracker::REL_POSITION:
            grabbed_relative_position_ = tracker->getRelativePosition();
            break;
        default:
            break;
    }
    grabs_[target_prop] = tracker;
    updateGrabFromTracker(tracker, target_prop);
    return true;
}

bool InteractiveImageToken::registerLink(Tracker *tracker, int target_prop)
{
    token_grabbed_ = false;

    if(hasLink(target_prop)) {
        removeLink(target_prop);
    } else if (hasGrab(target_prop)) {
        removeGrab(target_prop);
    }

    return Trackable::registerLink(tracker, target_prop);
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
    prepareGeometryChange();
    name_ = n;
}

const QUuid &InteractiveImageToken::getUuid() const
{
    return uuid_;
}

const QRectF InteractiveImageToken::calculateBoundingRect() const
{
    QPainterPath p;
    p.addRect(marker_rect_);
    p.addRect(textRect());
    p.addRect(grab_rect_);
    p.addRect(uncover_rect_);
    return p.boundingRect();
}

float InteractiveImageToken::getUncoverRadius() const
{
    return uncover_radius_;
}

void InteractiveImageToken::setUncoverRadius(float r)
{
    uncover_radius_ = r;

    emit uncoverRadiusChanged();
}

float InteractiveImageToken::getGrabRadius() const
{
    return grab_radius_;
}

void InteractiveImageToken::setGrabRadius(float d)
{
    grab_radius_ = d;
    grab_rect_ = QRectF(-grab_radius_, -grab_radius_, grab_radius_*2, grab_radius_*2);
    prepareGeometryChange();
}

bool InteractiveImageToken::getShowGrabIndicator() const
{
    return show_grab_indicator_;
}

void InteractiveImageToken::setShowGrabIndicator(bool show)
{
    show_grab_indicator_ = show;
}

bool InteractiveImageToken::getShowUncoverIndicator() const
{
    return show_uncover_indicator_;
}

void InteractiveImageToken::setShowUncoverIndicator(bool show)
{
    show_uncover_indicator_ = show;
}

float InteractiveImageToken::getUncoverIndicatorRadius() const
{
    return uncover_radius_;
}

void InteractiveImageToken::setUncoverIndicatorRadius(float r)
{
    uncover_rect_ = QRectF(-uncover_indicator_radius,
                           -uncover_indicator_radius,
                           uncover_indicator_radius * 2,
                           uncover_indicator_radius * 2
                           );
    uncover_indicator_radius = r;
}

const QColor &InteractiveImageToken::getColor() const
{
    return color_;
}

void InteractiveImageToken::setColor(const QColor &clr)
{
    color_ = clr;
    prepareGeometryChange();
}

void InteractiveImageToken::setSize(const QSizeF &s)
{
    prepareGeometryChange();
    marker_rect_.setSize(s);
}

const QPointF InteractiveImageToken::centerPos() const
{
    return mapToScene(markerRect().center());
}

void InteractiveImageToken::setUncoverPos(const QPointF &pos)
{
    setPos(pos);
    emit hasMoved();
}

bool InteractiveImageToken::ensureGrabbable(Tracker *tracker, int target_prop)
{
    Q_UNUSED(tracker);
    Q_UNUSED(target_prop);
    // TODO: extend with interatcion range
    return true;
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
