#include "view.h"

#include <QGraphicsScene>
#include <QBoxLayout>
#include <QKeyEvent>
#include <QDebug>
#include <QScrollBar>
#include <QCoreApplication>

#include "image_item.h"
#include "interactive/interactive_image.h"

namespace Image {

View::View(QWidget *parent)
    : QGraphicsView(parent)
    , item_(0)
    , context_menu_(0)
{
    setScene(new QGraphicsScene(this));
    setStyleSheet("background-color: #373738; color: white;");
    initContextMenu();
}

View::~View()
{
    context_menu_->deleteLater();
}

void View::setItem(ImageItem* it)
{
    setItem((QGraphicsItem*) it);
}

void View::setItem(QGraphicsItem* item)
{
    clear();
    scene()->addItem(item);
    scene()->setSceneRect(item->boundingRect());
    scaleContentsToViewport();
    item_ = item;
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

void View::onOverlayMapFog()
{
    auto it = qgraphicsitem_cast<ImageItem*>(item_);
    if(it) {
        QString path = it->getPath();
        setItem(new InteractiveImage(path, it->boundingRect().size().toSize()));
    }
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

void View::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    if(!event->isAccepted()) {
        if(event->button() == Qt::RightButton) {
            context_menu_->popup(event->globalPos());
            event->accept();
        }
    }
}

void View::initContextMenu()
{
    context_menu_ = new QMenu;

    QAction* cover_image = new QAction(tr("Make Interactive"));
    connect(cover_image, SIGNAL(triggered()),
            this, SLOT(onOverlayMapFog()));

    context_menu_->addAction(cover_image);
}

} // namespace Image
