#include "tuio_graphics_view.h"

#include <QResizeEvent>
#include <QGraphicsItem>

TuioGraphicsView::TuioGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{
   setScene(new QGraphicsScene(0,0,800,600, this));
}

void TuioGraphicsView::resizeEvent(QResizeEvent *e)
{
    QGraphicsView::resizeEvent(e);
    if(!e->isAccepted())
        return;
    QRectF r = scene()->sceneRect();
    QMap<QGraphicsItem*, QPointF> new_pos;
    foreach(auto it, scene()->items()) {
        new_pos[it] = QPointF(
            (it->pos().x() / r.width()) * e->size().width(),
            (it->pos().y() / r.height()) * e->size().height()
        );
    }
    r.setWidth(e->size().width());
    r.setHeight(e->size().height());
    scene()->setSceneRect(r);
    foreach(auto it, new_pos.keys()) {
        it->setPos(new_pos[it]);
    }
}
