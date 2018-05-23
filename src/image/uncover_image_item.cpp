#include "uncover_image_item.h"

#include <QFileDialog>
#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>


UncoverImageItem::UncoverImageItem(const QSize &size, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , result_image_(0)
    , result_size_(size)
    , src_image_(0)
    , drawing_(false)
    , paths_()
    , token_paths_()
    , click_pos_()
    , line_()
    , context_menu_(0)
{
    result_image_ = new QImage(result_size_, QImage::Format_ARGB32_Premultiplied);
    src_image_ = new QImage;
    loadImage();
    calcResultImage();
    initContextMenu();
}

UncoverImageItem::UncoverImageItem(const QString& path, const QSize &size, QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , result_image_(0)
    , result_size_(size)
    , src_image_(0)
    , drawing_(false)
    , paths_()
    , token_paths_()
    , click_pos_()
    , line_()
    , context_menu_(0)
{
    result_image_ = new QImage(result_size_, QImage::Format_ARGB32_Premultiplied);
    src_image_ = new QImage;
    loadFileIntoImage(path, src_image_);
    calcResultImage();
    initContextMenu();
}

UncoverImageItem::~UncoverImageItem()
{
    delete src_image_;
    delete result_image_;
}

QRectF UncoverImageItem::boundingRect() const
{
    return QRectF(0, 0 , result_size_.width(), result_size_.height());
}

void UncoverImageItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->fillRect(boundingRect(), Qt::black);
    painter->drawPixmap(boundingRect().toRect(), QPixmap::fromImage(*result_image_));
}

UncoverTokenItem *UncoverImageItem::getToken(const QUuid &uuid)
{
    foreach(auto it, token_paths_.keys())
        if(uuid == it->getUuid())
            return it;
    return 0;
}

void UncoverImageItem::addToken(UncoverTokenItem *it)
{
    token_paths_[it] = QPainterPath(it->pos());
    connect(it, UncoverTokenItem::hasMoved,
            this, [=](){onHasMoved(it->getUuid());});
}

void UncoverImageItem::loadImage()
{
    QString file_name = QFileDialog::getOpenFileName();
    if (file_name.isEmpty())
        return;
    loadFileIntoImage(file_name, src_image_);
}

void UncoverImageItem::onHasMoved(const QUuid &uuid)
{
    UncoverTokenItem* it = getToken(uuid);
    if(it) {
        token_paths_[it].lineTo(it->pos());
        calcResultImage();
    }
}

void UncoverImageItem::onCreateToken()
{
    UncoverTokenItem* it = new UncoverTokenItem(this);
    it->setPos(it->mapFromScene(click_pos_));
    addToken(it);
}

void UncoverImageItem::initContextMenu()
{
    context_menu_ = new QMenu;

    QAction* create_token = new QAction(tr("Create Token"));
    connect(create_token, SIGNAL(triggered()),
            this, SLOT(onCreateToken()));

    context_menu_->addAction(create_token);
}

void UncoverImageItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mousePressEvent(event);
    if(event->isAccepted())
        return;

    foreach(auto it, token_paths_.keys()) {
        if(it->contains(it->mapFromParent(event->pos()))) {
            event->setPos(it->mapFromParent(event->pos()));
            it->mousePressEventAlt(event);
            return;
        }
    }

    if(event->button() == Qt::LeftButton) {
        drawing_ = true;
        paths_.append(QPainterPath(event->pos()));
        event->accept();
    }
    else if(event->button() == Qt::RightButton) {
        click_pos_ = event->pos();
        context_menu_->popup(click_pos_.toPoint());
        event->accept();
    }
}

void UncoverImageItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mouseMoveEvent(event);
    if(event->isAccepted())
        return;

    foreach(auto it, token_paths_.keys()) {
        if(it->contains(it->mapFromParent(event->pos()))) {
            event->setPos(it->mapFromParent(event->pos()));
            it->mouseMoveEventAlt(event);
            return;
        }
    }

    if(drawing_) {
        paths_.back().lineTo(event->pos());
        calcResultImage();
    }
}

void UncoverImageItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mouseReleaseEvent(event);

    foreach(auto it, token_paths_.keys()) {
        if(it->contains(it->mapFromParent(event->pos()))) {
            event->setPos(it->mapFromParent(event->pos()));
            it->mouseReleaseEventAlt(event);
            return;
        }
    }

    drawing_ = false;
}

void UncoverImageItem::calcResultImage()
{
    prepareGeometryChange();
    QPainter p(result_image_);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.fillRect(result_image_->rect(), Qt::transparent);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    p.setPen(QPen(Qt::black, 100, Qt::SolidLine, Qt::RoundCap));
    foreach(auto path, token_paths_.values())
        p.drawPath(path);
    foreach(auto path, paths_)
        p.drawPath(path);
    p.setCompositionMode(QPainter::CompositionMode_SourceIn);
    p.drawImage(0, 0, *src_image_);
    p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    p.end();
}

const QPoint UncoverImageItem::imagePos(const QImage& img) const
{
    return QPoint((result_size_.width() - img.width()) / 2,
                  (result_size_.height() - img.height()) / 2);
}

void UncoverImageItem::loadFileIntoImage(const QString &file, QImage *img)
{
    // load & scale the image to given size
    img->load(file);
    *img = img->scaled(result_size_, Qt::KeepAspectRatio);

    QImage fixed_image(result_size_, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&fixed_image);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(fixed_image.rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(imagePos(*img), *img);
    painter.end();

    *img = fixed_image;
}
