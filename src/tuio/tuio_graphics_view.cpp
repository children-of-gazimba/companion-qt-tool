#include "tuio_graphics_view.h"

#include <QResizeEvent>

TuioGraphicsView::TuioGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{
   setScene(new QGraphicsScene(0,0,800,600, this));
}

void TuioGraphicsView::resizeEvent(QResizeEvent *e)
{
    QGraphicsView::resizeEvent(e);
    if(e->isAccepted()) {
        QRectF r = scene()->sceneRect();
        r.setWidth(e->size().width());
        r.setHeight(e->size().height());
        scene()->setSceneRect(r);
    }
}
