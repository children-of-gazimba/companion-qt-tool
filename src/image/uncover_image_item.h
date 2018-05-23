#ifndef UNCOVER_IMAGE_ITEM_H
#define UNCOVER_IMAGE_ITEM_H

#include <QGraphicsObject>
#include <QImage>
#include <QMap>

#include "uncover_token_item.h"

class UncoverImageItem : public QGraphicsObject
{
    Q_OBJECT
public:
    UncoverImageItem(const QSize& size, QGraphicsItem* parent = 0);
    UncoverImageItem(const QString& path, const QSize& size, QGraphicsItem* parent = 0);
    virtual ~UncoverImageItem();

    /**
     * See BC.
    */
    virtual QRectF boundingRect() const;

    /**
     * See BC.
    */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    UncoverTokenItem* getToken(const QUuid& uuid);
    void addToken(UncoverTokenItem* it);

public slots:
    void loadImage();

protected slots:
    void onHasMoved(const QUuid&);
    void onCreateToken();

protected:
    void initContextMenu();

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void calcResultImage();
    const QPoint imagePos(const QImage& img) const;
    void loadFileIntoImage(const QString& file, QImage* img);

    QImage* result_image_;
    QSize result_size_;
    QImage* src_image_;
    QImage* dst_image_;
    bool drawing_;
    QList<QPainterPath> paths_;
    QMap<UncoverTokenItem*, QPainterPath> token_paths_;
    QPointF click_pos_;
    QLineF line_;
    QMenu* context_menu_;
};

#endif // UNCOVER_IMAGE_ITEM_H