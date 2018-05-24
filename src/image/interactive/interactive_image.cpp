#include "interactive_image.h"

#include <QFileDialog>
#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QGraphicsScene>


InteractiveImage::InteractiveImage(const QSize &size, QGraphicsItem* parent)
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
    , actions_()
    , all_uncovered_(true)
    , menu_bar_extension_(0)
{
    result_image_ = new QImage(result_size_, QImage::Format_ARGB32_Premultiplied);
    src_image_ = new QImage;
    loadImage();
    calcResultImage();
    initContextMenu();
}

InteractiveImage::InteractiveImage(const QString& path, const QSize &size, QGraphicsItem *parent)
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
    , actions_()
    , all_uncovered_(true)
    , menu_bar_extension_(0)
{
    result_image_ = new QImage(result_size_, QImage::Format_ARGB32_Premultiplied);
    src_image_ = new QImage;
    loadFileIntoImage(path, src_image_);
    calcResultImage();
    initContextMenu();
}

InteractiveImage::~InteractiveImage()
{
    delete src_image_;
    delete result_image_;
    foreach(auto it, token_paths_.keys())
        it->deleteLater();
    if(menu_bar_extension_)
        menu_bar_extension_->deleteLater();
}

QRectF InteractiveImage::boundingRect() const
{
    return QRectF(0, 0 , result_size_.width(), result_size_.height());
}

void InteractiveImage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->fillRect(boundingRect(), Qt::black);
    painter->drawPixmap(boundingRect().toRect(), QPixmap::fromImage(*result_image_));
}

InteractiveImageToken *InteractiveImage::getToken(const QUuid &uuid)
{
    foreach(auto it, token_paths_.keys())
        if(uuid == it->getUuid())
            return it;
    return 0;
}

void InteractiveImage::addToken(InteractiveImageToken *it)
{
    scene()->addItem(it);
    linkToken(it);
    calcResultImage();
}

QMenu *InteractiveImage::getMenuBarExtension()
{
    if(menu_bar_extension_)
        return menu_bar_extension_;
    menu_bar_extension_ = new QMenu(tr("Actions"));
    QMenu* fog_menu = menu_bar_extension_->addMenu(tr("Fog"));
    fog_menu->addAction(actions_["cover"]);
    fog_menu->addAction(actions_["uncover"]);
    menu_bar_extension_->addMenu(fog_menu);
    return menu_bar_extension_;
}

void InteractiveImage::linkToken(InteractiveImageToken *it)
{
    token_paths_[it] = QPainterPath(it->centerPos());
    connect(it, InteractiveImageToken::hasMoved,
            this, [=](){onHasMoved(it->getUuid());});
}

void InteractiveImage::loadImage()
{
    QString file_name = QFileDialog::getOpenFileName();
    if (file_name.isEmpty())
        return;
    loadFileIntoImage(file_name, src_image_);
}

void InteractiveImage::onHasMoved(const QUuid &uuid)
{
    InteractiveImageToken* it = getToken(uuid);
    if(it) {
        token_paths_[it].lineTo(it->centerPos());
        calcResultImage();
    }
}

void InteractiveImage::onCreateToken()
{
    if(!scene())
        return;

    InteractiveImageToken* it = new InteractiveImageToken(QSizeF(50,50));
    it->setUncoverRadius(100);
    QPointF spawn_pos(it->mapFromScene(click_pos_));
    spawn_pos.setX(spawn_pos.x()-it->boundingRect().width()/2.0f);
    spawn_pos.setY(spawn_pos.y()-it->boundingRect().height()/2.0f);
    it->setPos(spawn_pos);
    addToken(it);
}

void InteractiveImage::onUncoverAll()
{
    setAllUncovered(true);
}

void InteractiveImage::onCoverAll()
{
    clearAllPaths();
    setAllUncovered(false);
}

void InteractiveImage::clearAllPaths()
{
    paths_.clear();
    foreach (auto it, token_paths_.keys()) {
        token_paths_[it] = QPainterPath(it->centerPos());
    }
}

void InteractiveImage::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mousePressEvent(event);
    if(event->isAccepted())
        return;

    if(event->button() == Qt::LeftButton) {
        drawing_ = true;
        paths_.append(QPainterPath(event->pos()));
        event->accept();
    }
    else if(event->button() == Qt::RightButton) {
        event->accept();
    }
}

void InteractiveImage::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mouseMoveEvent(event);
    if(event->isAccepted())
        return;

    if(drawing_) {
        paths_.back().lineTo(event->pos());
        calcResultImage();
    }
}

void InteractiveImage::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mouseReleaseEvent(event);
    drawing_ = false;
}

void InteractiveImage::contextMenuEvent(QGraphicsSceneContextMenuEvent *e)
{
    click_pos_ = e->pos();
    context_menu_->popup(e->screenPos());
}

void InteractiveImage::calcResultImage()
{
    prepareGeometryChange();
    QPainter p(result_image_);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.fillRect(result_image_->rect(), Qt::transparent);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    if(all_uncovered_) {
        p.fillRect(result_image_->rect(), Qt::black);
    }
    else {
        foreach(auto it, token_paths_.keys()) {
            p.setBrush(Qt::NoBrush);
            p.setPen(QPen(Qt::black, it->getUncoverRadius(), Qt::SolidLine, Qt::RoundCap));
            p.drawPath(token_paths_[it]);
            p.setBrush(Qt::black);
            p.setPen(Qt::NoPen);
            QRectF uncover_area(it->mapRectToScene(it->uncoverBoundingRect()));
            uncover_area.setX(uncover_area.x()-uncover_area.width()/2.0f);
            uncover_area.setY(uncover_area.y()-uncover_area.height()/2.0f);
            p.drawEllipse(uncover_area);
        }
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(Qt::black, 100, Qt::SolidLine, Qt::RoundCap));
        foreach(auto path, paths_)
            p.drawPath(path);
    }
    p.setCompositionMode(QPainter::CompositionMode_SourceIn);
    p.drawImage(0, 0, *src_image_);
    p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    p.end();
}

const QPoint InteractiveImage::imagePos(const QImage& img) const
{
    return QPoint((result_size_.width() - img.width()) / 2,
                  (result_size_.height() - img.height()) / 2);
}

void InteractiveImage::loadFileIntoImage(const QString &file, QImage *img)
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

void InteractiveImage::setAllUncovered(bool state)
{
    all_uncovered_ = state;
    actions_["uncover"]->setEnabled(!all_uncovered_);
    calcResultImage();
}

void InteractiveImage::initContextMenu()
{
    context_menu_ = new QMenu;

    actions_["create_token"] = new QAction(tr("Create Token"));
    connect(actions_["create_token"], SIGNAL(triggered()),
            this, SLOT(onCreateToken()));

    actions_["cover"] = new QAction(tr("Overlay Map Fog"));
    connect(actions_["cover"], SIGNAL(triggered()),
            this, SLOT(onCoverAll()));

    actions_["uncover"] = new QAction(tr("Clear Map Fog"));
    connect(actions_["uncover"], SIGNAL(triggered()),
            this, SLOT(onUncoverAll()));
    actions_["uncover"]->setEnabled(!all_uncovered_);

    context_menu_->addAction(actions_["create_token"]);
    context_menu_->addSeparator();
    context_menu_->addAction(actions_["cover"]);
    context_menu_->addAction(actions_["uncover"]);
}
