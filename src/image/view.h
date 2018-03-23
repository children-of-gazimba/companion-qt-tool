#ifndef IMAGE_VIEW_H
#define IMAGE_VIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QShowEvent>

namespace Image {

class View : public QGraphicsView
{
    Q_OBJECT
public:
    explicit View(QWidget *parent = 0);
    void setItem(QGraphicsItem*);

signals:

public slots:
    void clear();
    void scaleContentsToViewport();

private:
    void resizeEvent(QResizeEvent *event);
    const QRectF getVisibleRect() const;
    void keyPressEvent(QKeyEvent *event);
};

} // namespace Image

#endif // IMAGE_VIEW_H
