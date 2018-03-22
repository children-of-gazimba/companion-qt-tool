#include "view.h"

#include <QGraphicsScene>
#include <QBoxLayout>
#include <QKeyEvent>
#include <QDebug>
#include <QScrollBar>
#include <QCoreApplication>

namespace Image {

View::View(QWidget *parent)
    : QWidget(parent)
    , view_(0)
{
    initWidgets();
    initLayout();
    setStyleSheet("background-color: #373738; color: white;");
}

void View::setItem(QGraphicsItem* item)
{
    clear();
    view_->scene()->addItem(item);
    view_->scene()->setSceneRect(item->boundingRect());
    scaleContentsToViewport();
}

void View::clear()
{
    view_->scene()->clear();
    view_->scene()->setSceneRect(QRectF(0,0,0,0));
}

void View::scaleContentsToViewport()
{
    QRectF visible_rect = getVisibleRect();
    qreal scale_factor = qMin(
        visible_rect.width() / view_->scene()->sceneRect().width(),
        visible_rect.height() / view_->scene()->sceneRect().height()
    );
    view_->scale(scale_factor, scale_factor);
}

void View::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    scaleContentsToViewport();
}

const QRectF View::getVisibleRect() const
{
    QPointF tl(view_->horizontalScrollBar()->value(), view_->verticalScrollBar()->value());
    QPointF br = tl + view_->viewport()->rect().bottomRight();
    QMatrix mat = view_->matrix().inverted();
    return mat.mapRect(QRectF(tl,br));
}

void View::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
        case '-':
            view_->scale(0.9,0.9);
            break;
        case '+':
            view_->scale(1.1,1.1);
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

void View::initWidgets()
{
    QGraphicsScene* scene = new QGraphicsScene(this);
    view_ = new QGraphicsView(scene);
}

void View::initLayout()
{
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(view_);
    layout->setMargin(0);
    setLayout(layout);
}

} // namespace Image
