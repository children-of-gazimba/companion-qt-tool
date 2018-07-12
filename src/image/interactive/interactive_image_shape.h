#ifndef INTERACTIVE_IMAGE_SHAPE_H
#define INTERACTIVE_IMAGE_SHAPE_H

#include <QGraphicsPathItem>
#include <QObject>

class InteractiveImageShape : public QGraphicsObject
{
    Q_OBJECT

    Q_INTERFACES(QGraphicsItem) // so instances can be casted using qobject_cast

public:
    enum { Type = UserType + 80 };
    virtual int type() const { return Type; }

public:
    InteractiveImageShape(const QPainterPath& path, QGraphicsItem* parent = 0);
    virtual ~InteractiveImageShape();

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget);
    virtual QPainterPath shape() const;

    void setPath(const QPainterPath& p);

protected:
    QPainterPath path_;
};

#endif // INTERACTIVE_IMAGE_SHAPE_H
