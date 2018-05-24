#ifndef INTERACTIVE_IMAGE_H
#define INTERACTIVE_IMAGE_H

#include <QGraphicsObject>
#include <QImage>
#include <QMap>

#include "interactive_image_token.h"

class QAction;

class InteractiveImage : public QGraphicsObject
{
    Q_OBJECT
public:
    InteractiveImage(const QSize& size, QGraphicsItem* parent = 0);
    InteractiveImage(const QString& path, const QSize& size, QGraphicsItem* parent = 0);
    virtual ~InteractiveImage();

    /**
     * See BC.
    */
    virtual QRectF boundingRect() const;

    /**
     * See BC.
    */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    InteractiveImageToken* getToken(const QUuid& uuid);
    void linkToken(InteractiveImageToken* it);

public slots:
    void loadImage();

protected slots:
    void onHasMoved(const QUuid&);
    void onCreateToken();
    void onUncoverAll();
    void onCoverAll();

protected:
    void clearAllPaths();
    void calcResultImage();
    const QPoint imagePos(const QImage& img) const;
    void loadFileIntoImage(const QString& file, QImage* img);
    void setAllUncovered(bool state);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* e);

    void initContextMenu();

    QImage* result_image_;
    QSize result_size_;
    QImage* src_image_;
    QImage* dst_image_;
    bool drawing_;
    QList<QPainterPath> paths_;
    QMap<InteractiveImageToken*, QPainterPath> token_paths_;
    QPointF click_pos_;
    QLineF line_;
    QMenu* context_menu_;
    QMap<QString, QAction*> actions_;
    bool all_uncovered_;
};

#endif // INTERACTIVE_IMAGE_H
