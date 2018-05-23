#ifndef UNCOVER_TOKEN_ITEM_H
#define UNCOVER_TOKEN_ITEM_H

#include <QGraphicsObject>
#include <QUuid>

class UncoverTokenItem : public QGraphicsObject
{
    Q_OBJECT

    Q_INTERFACES(QGraphicsItem) // so instances can be casted using qobject_cast

public:
    enum State {
        IDLE,
        SELECTED,
        MOVE
    };

public:
    explicit UncoverTokenItem(QGraphicsItem *parent = nullptr);

    /**
     * See BC.
    */
    virtual QRectF boundingRect() const;

    /**
     * See BC.
    */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    const QUuid& getUuid() const;

    virtual void mousePressEventAlt(QGraphicsSceneMouseEvent* e);
    virtual void mouseReleaseEventAlt(QGraphicsSceneMouseEvent* e);
    virtual void mouseMoveEventAlt(QGraphicsSceneMouseEvent* e);

signals:
    void hasMoved();

public slots:

protected:
    void setState(State state);

    QRectF bounding_rect_;
    State state_;
    QUuid uuid_;
    QPointF click_offset_;
};

#endif // UNCOVER_TOKEN_ITEM_H
