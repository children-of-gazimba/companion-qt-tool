#ifndef INTERACTIVE_IMAGE_H
#define INTERACTIVE_IMAGE_H

#include <QGraphicsObject>
#include <QImage>
#include <QMap>
#include <QSet>

#include "interactive_image_token.h"

class QAction;

class InteractiveImage : public QGraphicsObject
{
    Q_OBJECT
public:
    enum { Type = UserType + 78 };
    virtual int type() const { return Type; }

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

    /*
    * Returns the Token referenced by given uuid,
    * or 0 if it doesn't exist.
    */
    InteractiveImageToken* getToken(const QUuid& uuid);

    /*
    * Adds given token to the view
    * and connects it to the uncover functionality.
    */
    void addToken(InteractiveImageToken* it);

    /*
    * Returns the QMenu which can be used as a
    * MenuBar extension. All actions
    * are also available from the context menu.
    */
    QMenu* getMenuBarExtension();

    /**
     * @brief getTokens
     * @return
     */
    const QList<InteractiveImageToken*> getTokens() const;

    const QSet<QString>& getTrackerNames() const;

    bool addTrackerName(const QString& n);

    bool removeTrackerName(const QString& n);

signals:
    void tokenAdded(InteractiveImageToken* it);

public slots:
    void loadImage();

protected slots:
    void onHasMoved(const QUuid&);
    void onCreateToken();
    void onCreateToken(const QString&);
    void onUncoverAll();
    void onCoverAll();
    void onTrackerAdded(QString const&);
    void onTrackerRemoved(QString const&);

protected:    
    void linkToken(InteractiveImageToken* it);
    void clearAllPaths();
    void scheduleCalcResultImage();
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
    QMenu* menu_bar_extension_;
    bool need_calc_;
    QSet<QString> tracker_names_;
};

#endif // INTERACTIVE_IMAGE_H
