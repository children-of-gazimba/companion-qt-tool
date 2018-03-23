#include "view.h"

#include <QGraphicsScene>
#include <QBoxLayout>
#include <QKeyEvent>
#include <QDebug>
#include <QScrollBar>
#include <QCoreApplication>

namespace Image {

View::View(QWidget *parent)
    : QGraphicsView(parent)
{
    setScene(new QGraphicsScene(this));
    setStyleSheet("background-color: #373738; color: white;");
}

void View::setItem(QGraphicsItem* item)
{
    clear();
    scene()->addItem(item);
    scene()->setSceneRect(item->boundingRect());
    scaleContentsToViewport();
}

void View::clear()
{
    scene()->clear();
    scene()->setSceneRect(QRectF(0,0,0,0));
}

void View::scaleContentsToViewport()
{
    QRectF visible_rect = getVisibleRect();
    qreal scale_factor = qMin(
        visible_rect.width() / scene()->sceneRect().width(),
        visible_rect.height() / scene()->sceneRect().height()
    );
    scale(scale_factor, scale_factor);
}

void View::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    scaleContentsToViewport();
}

const QRectF View::getVisibleRect() const
{
    QPointF tl(horizontalScrollBar()->value(), verticalScrollBar()->value());
    QPointF br = tl + viewport()->rect().bottomRight();
    QMatrix mat = matrix().inverted();
    return mat.mapRect(QRectF(tl,br));
}

void View::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
        case '-':
            scale(0.9,0.9);
            break;
        case '+':
            scale(1.1,1.1);
            break;
        case ' ':
            if (windowState().testFlag(Qt::WindowFullScreen)) {
                showNormal();
            }
            else {
                showFullScreen();
            }
            break;
    }
    QWidget::keyPressEvent(event);
}

} // namespace Image
