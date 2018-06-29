#include "tuio_token_tracker.h"

#include <QDebug>

TuioTokenTracker::TuioTokenTracker()
    : Tracker()
    , id_(-1)
    , class_id_(-1)
    , flip_x_(false)
    , flip_y_(false)
{}

TuioTokenTracker::TuioTokenTracker(int class_id, int id)
    : Tracker()
    , id_(id)
    , class_id_(class_id)
    , flip_x_(false)
    , flip_y_(false)
{}

void TuioTokenTracker::set(const Tracker *other)
{
    if(other->trackerType() == TuioTokenTracker::TrackerType) {
        auto other_token = static_cast<const TuioTokenTracker*>(other);
        id_ = other_token->getId();
        class_id_ = other_token->getClassId();
        flip_x_ = other_token->getXFlipped();
        flip_y_ = other_token->getYFlipped();
    }
    Tracker::set(other);
}

void TuioTokenTracker::setRelativePosition(const QPointF &p)
{
    QPointF rel_p(p);
    if(flip_x_)
        rel_p.setX(1-p.x());
    if(flip_y_)
        rel_p.setY(1-p.y());
    Tracker::setRelativePosition(rel_p);
}

const QList<int> TuioTokenTracker::supportedTargetProperties() const
{
    QList<int> p;
    p.append(Tracker::REL_POSITION);
    p.append(Tracker::ROTATION);
    return p;
}

bool TuioTokenTracker::set(const QTuioToken &t)
{
    if(!isCompatible(t))
        return false;
    setRelativePosition(QPointF(t.x(), t.y()));
    setRotation(qRadiansToDegrees(t.angle()));
    return true;
}

int TuioTokenTracker::getId() const
{
    return id_;
}

int TuioTokenTracker::getClassId() const
{
    return class_id_;
}

void TuioTokenTracker::setXFlipped(bool enabled)
{
    if(enabled == flip_x_)
        return;
    flip_x_ = enabled;
    QPointF rel_p = getRelativePosition();
    rel_p.setY(1-rel_p.y());
    setRelativePosition(rel_p);
}

bool TuioTokenTracker::getXFlipped() const
{
    return flip_x_;
}

void TuioTokenTracker::setYFlipped(bool enabled)
{
    if(enabled == flip_y_)
        return;
    flip_y_ = enabled;
    QPointF rel_p = getRelativePosition();
    rel_p.setX(1-rel_p.x());
    setRelativePosition(rel_p);
}

bool TuioTokenTracker::getYFlipped() const
{
    return flip_y_;
}

bool TuioTokenTracker::isValid() const
{
    return id_ >= 0 || class_id_ >= 0;
}

bool TuioTokenTracker::isCompatible(const QTuioToken &t)
{
    return t.id() == id_ || t.classId() == class_id_;
}
