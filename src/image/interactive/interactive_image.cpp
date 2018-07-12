#include "interactive_image.h"

#include <QFileDialog>
#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QGraphicsScene>
#include <QMessageBox>

#include "resources/lib.h"

InteractiveImage::InteractiveImage(const QSize &size, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , result_image_(0)
    , result_size_(size)
    , src_image_(0)
    , drawing_(false)
    , paths_()
    , token_paths_()
    , shapes_()
    , uncovered_shapes_()
    , merged_shape_()
    , click_pos_()
    , line_()
    , context_menu_(0)
    , actions_()
    , all_uncovered_(true)
    , menu_bar_extension_(0)
    , need_calc_(false)
    , tracker_names_()
    , creating_shape_(false)
    , shape_rect_()
{
    merged_shape_.setFillRule(Qt::WindingFill);
    result_image_ = new QImage(result_size_, QImage::Format_ARGB32_Premultiplied);
    src_image_ = new QImage;
    loadImage();
    scheduleCalcResultImage();
    initContextMenu();
    connect(Resources::Lib::TRACKER_MODEL, &TrackerTableModel::trackerAdded,
            this, &InteractiveImage::onTrackerAdded);
    connect(Resources::Lib::TRACKER_MODEL, &TrackerTableModel::trackerRemoved,
            this, &InteractiveImage::onTrackerRemoved);
    foreach(auto tracker, Resources::Lib::TRACKER_MODEL->getTrackers())
        addTrackerName(tracker->getName());
}

InteractiveImage::InteractiveImage(const QString& path, const QSize &size, QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , result_image_(0)
    , result_size_(size)
    , src_image_(0)
    , drawing_(false)
    , paths_()
    , token_paths_()
    , shapes_()
    , uncovered_shapes_()
    , merged_shape_()
    , click_pos_()
    , line_()
    , context_menu_(0)
    , actions_()
    , all_uncovered_(true)
    , menu_bar_extension_(0)
    , need_calc_(false)
    , tracker_names_()
    , creating_shape_(false)
    , shape_rect_()
{
    merged_shape_.setFillRule(Qt::WindingFill);
    result_image_ = new QImage(result_size_, QImage::Format_ARGB32_Premultiplied);
    src_image_ = new QImage;
    loadFileIntoImage(path, src_image_);
    scheduleCalcResultImage();
    initContextMenu();
    connect(Resources::Lib::TRACKER_MODEL, &TrackerTableModel::trackerAdded,
            this, &InteractiveImage::onTrackerAdded);
    connect(Resources::Lib::TRACKER_MODEL, &TrackerTableModel::trackerRemoved,
            this, &InteractiveImage::onTrackerRemoved);
    foreach(auto tracker, Resources::Lib::TRACKER_MODEL->getTrackers())
        addTrackerName(tracker->getName());
}

InteractiveImage::~InteractiveImage()
{
    delete src_image_;
    delete result_image_;
    foreach(auto it, token_paths_.keys())
        it->deleteLater();
    token_paths_.clear();
    foreach(auto sh, shapes_)
        sh->deleteLater();
    shapes_.clear();
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
    if(need_calc_)
        calcResultImage();

    painter->fillRect(boundingRect(), QColor(55,55,56));

    if(creating_shape_ && opacity() > 0.7f)
        setOpacity(0.7f);
    else if(!creating_shape_ && opacity() < 1.0f)
        setOpacity(1.0f);

    painter->drawPixmap(boundingRect().toRect(), QPixmap::fromImage(*result_image_));

    QPen p(painter->pen());
    p.setWidth(5);
    p.setColor(QColor(155,155,156));
    painter->setPen(p);

    if(!all_uncovered_) {
        painter->drawPath(merged_shape_);
        //painter->drawPath(merged_shape_);
        /*foreach(auto sh, shapes_.toSet().subtract(uncovered_shapes_))
            painter->fillPath(sh->shape(), QColor(55,55,56));*/
    }

    p.setColor(QColor(55,55,56));
    painter->setPen(p);

    if(creating_shape_ && !shape_rect_.isEmpty()) {
        painter->fillRect(shape_rect_, QColor(155,155,156));
        painter->drawRect(shape_rect_);
    }
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
    scheduleCalcResultImage();
    connect(it, &InteractiveImageToken::destroyed,
            this, &InteractiveImage::onTokenDeleted);
    emit tokenAdded(it);
}

void InteractiveImage::addShape(InteractiveImageShape *sh)
{
    scene()->addItem(sh);
    shapes_.append(sh);
    if(all_uncovered_) {
        sh->setOpacity(0.7f);
        sh->setFlag(QGraphicsItem::ItemIsSelectable, true);
    }
    else {
        sh->hide();
        prepareGeometryChange();
        merged_shape_.addPath(sh->shape());
        merged_shape_ = merged_shape_.simplified();
        merged_shape_.setFillRule(Qt::WindingFill);
    }
    connect(sh, &InteractiveImageShape::destroyed,
            this, &InteractiveImage::onShapeDeleted);
    emit shapeAdded(sh);
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

const QList<InteractiveImageToken *> InteractiveImage::getTokens() const
{
    return token_paths_.keys();
}

const QSet<QString> &InteractiveImage::getTrackerNames() const
{
    return tracker_names_;
}

bool InteractiveImage::addTrackerName(const QString &n)
{
    if(tracker_names_.contains(n) || actions_.contains(n))
        return false;
    tracker_names_.insert(n);
    actions_[n] = new QAction("Create '" + n + "' Token", this);
    context_menu_->addAction(actions_[n]);
    connect(actions_[n], &QAction::triggered,
            this, [=](){onCreateToken(n);});
    return true;
}

bool InteractiveImage::removeTrackerName(const QString &n)
{
    if(!tracker_names_.contains(n))
        return false;
    tracker_names_.remove(n);
    context_menu_->removeAction(actions_[n]);
    actions_[n]->deleteLater();
    actions_.remove(n);
    return true;
}

void InteractiveImage::drawTokenPath(QPainter *painter, InteractiveImageToken *token, bool draw_thin)
{
    QPen p(Qt::black, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    if(!draw_thin) {
        p.setWidth(token->getUncoverRadius());
        if(token->getUncoverRadius() > token_paths_[token].boundingRect().width() &&
           token->getUncoverRadius() > token_paths_[token].boundingRect().height())
        {
            painter->setBrush(Qt::black);
            painter->setPen(Qt::NoPen);
            QRectF uncover_area(token->mapRectToScene(token->uncoverBoundingRect()));
            painter->drawEllipse(uncover_area);
        }
    }
    drawTokenPath(painter, token, p, QBrush(Qt::NoBrush));
}

void InteractiveImage::drawTokenPath(QPainter *painter, InteractiveImageToken* token, const QPen &pen, const QBrush& brush)
{
    if(!token_paths_.contains(token))
        return;
    painter->setBrush(brush);
    painter->setPen(pen);
    painter->drawPath(token_paths_[token]);
}

void InteractiveImage::drawUncoveredShapes(QPainter *painter)
{
    drawUncoveredShapes(
        painter,
        QPen(Qt::black, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin),
        QBrush(Qt::black)
    );
}

void InteractiveImage::drawUncoveredShapes(QPainter *painter, const QPen &pen, const QBrush &b)
{
    painter->setPen(pen);
    painter->setBrush(b);
    foreach(auto path, paths_)
        painter->drawPath(path);
}

void InteractiveImage::linkToken(InteractiveImageToken *it)
{
    token_paths_[it] = QPainterPath(it->centerPos());
    connect(it, &InteractiveImageToken::hasMoved,
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
        if(!all_uncovered_)
            scheduleCalcResultImage();
    }
}

void InteractiveImage::onCreateToken()
{
    if(!scene())
        return;

    InteractiveImageToken* it = new InteractiveImageToken(QSizeF(50,50));
    it->setUncoverRadius(100);
    QPointF spawn_pos(it->mapFromScene(click_pos_));
    it->setPos(spawn_pos);
    addToken(it);
}

void InteractiveImage::onCreateToken(const QString &n)
{
    if(!scene())
        return;

    InteractiveImageToken* it = new InteractiveImageToken(QSizeF(50,50));
    it->setUncoverRadius(100);
    QPointF spawn_pos(it->mapFromScene(click_pos_));
    it->setPos(spawn_pos);
    addToken(it);

    it->setTrackableName(n);
    it->setName(n);

    if(n.size() > 0) {
        foreach(auto tracker, Resources::Lib::TRACKER_MODEL->getTrackers()) {
            if(it->getTrackableName().compare(tracker->getName()) == 0) {
                tracker->link(it, Tracker::REL_POSITION);
                tracker->link(it, Tracker::ROTATION);
            }
        }
    }
}

void InteractiveImage::onMergeShapes()
{
    QList<InteractiveImageShape*> shapes_to_merge;
    foreach(auto sh, shapes_) {
        if(sh->isSelected())
            shapes_to_merge.append(sh);
    }
    if(shapes_to_merge.size() <= 1)
        return;
    QPainterPath p;
    p.setFillRule(Qt::WindingFill);
    foreach(auto sh, shapes_to_merge) {
        p.addPath(sh->shape());
        if(sh->scene())
            sh->scene()->removeItem(sh);
        shapes_.removeAll(sh);
        if(uncovered_shapes_.contains(sh))
            uncovered_shapes_.remove(sh);
        sh->deleteLater();
    }
    addShape(new InteractiveImageShape(p.simplified()));
}

void InteractiveImage::onCreateShape()
{
    prepareGeometryChange();
    creating_shape_ = true;
    shape_rect_ = QRectF();
    foreach(auto sh, shapes_)
        sh->setFlag(QGraphicsItem::ItemIsSelectable, false);
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

void InteractiveImage::onTrackerAdded(const QString &n)
{
    Tracker* t = Resources::Lib::TRACKER_MODEL->getTracker(n);
    if(t) {
        foreach(auto it, scene()->items()) {
            auto token = qgraphicsitem_cast<InteractiveImageToken*>(it);
            if(token && token->getTrackableName().size() > 0 && token->getTrackableName().compare(n) == 0) {
                t->link(token, Tracker::REL_POSITION);
                t->link(token, Tracker::ROTATION);
            }
        }
    }
    addTrackerName(n);
}

void InteractiveImage::onTrackerRemoved(const QString &n)
{
    removeTrackerName(n);
}

void InteractiveImage::onTokenDeleted(QObject* o)
{
    /*auto gi = qobject_cast<QGraphicsItem*>(o);
    if(!gi)
        return;*/
    auto token = static_cast<InteractiveImageToken*>(o);
    if(token && token_paths_.contains(token)) {
        scheduleCalcResultImage();
        paths_.append(token_paths_[token]);
        token_paths_.remove(token);
    }
}

void InteractiveImage::onShapeDeleted(QObject* o)
{
    /*auto gi = qobject_cast<QGraphicsItem*>(o);
    if(!gi)
        return;*/
    auto sh = static_cast<InteractiveImageShape*>(o);
    if(sh && (uncovered_shapes_.contains(sh) || shapes_.contains(sh))) {
        scheduleCalcResultImage();
        if(uncovered_shapes_.contains(sh))
            uncovered_shapes_.remove(sh);
        if(shapes_.contains(sh))
            shapes_.removeAll(sh);
    }
}

void InteractiveImage::clearAllPaths()
{
    paths_.clear();
    foreach (auto it, token_paths_.keys()) {
        token_paths_[it] = QPainterPath(it->centerPos());
    }
}

void InteractiveImage::scheduleCalcResultImage()
{
    prepareGeometryChange();
    need_calc_ = true;
}

void InteractiveImage::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mousePressEvent(event);
    if(event->isAccepted())
        return;

    if(event->button() == Qt::LeftButton) {
        foreach(auto sh, shapes_) {
            if(sh->isSelected())
                sh->setSelected(false);
        }
        if(!creating_shape_) {
            drawing_ = true;
            paths_.append(QPainterPath(event->pos()));
            event->accept();
        }
        else {
            prepareGeometryChange();
            click_pos_ = event->pos();
            shape_rect_.setTopLeft(event->pos());
            shape_rect_.setBottomRight(event->pos());
            event->accept();
        }
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
        scheduleCalcResultImage();
    }
    else if(creating_shape_) {
        prepareGeometryChange();
        shape_rect_ = orderedRect(click_pos_, event->pos());
    }
}

void InteractiveImage::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mouseReleaseEvent(event);
    drawing_ = false;
    if(creating_shape_)
        finalizeShapeDraw();
}

void InteractiveImage::contextMenuEvent(QGraphicsSceneContextMenuEvent *e)
{
    click_pos_ = e->pos();
    int num_selected_shapes = 0;
    foreach(auto sh, shapes_) {
        if(sh->isSelected()) {
            ++num_selected_shapes;
            if(num_selected_shapes > 1)
                break;
        }
    }
    actions_["merge_shapes"]->setEnabled(num_selected_shapes > 1);
    context_menu_->popup(e->screenPos());
}

void InteractiveImage::calcResultImage()
{
    // only update if re-calc needed
    if(!need_calc_)
        return;

    // setup paint context for result image
    // and make all image content transparent
    QPainter p(result_image_);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.fillRect(result_image_->rect(), Qt::transparent);

    // draw shapes which will be filled by source image
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    if(all_uncovered_) {
        // paint result image everywhere (all is visible)
        p.fillRect(result_image_->rect(), Qt::black);
    }
    else {
        // paint result image wherever there are:
        // - token paths
        // - unveiled shapes
        // - other shapes
        //   - e.g. mouse painted shapes, paths added from outside instance
        foreach(auto it, token_paths_.keys()) {
            drawTokenPath(&p, it);
            unveilShapes(it);
        }
        drawUncoveredShapes(&p);
    }

    // fill result image
    p.setCompositionMode(QPainter::CompositionMode_SourceIn);
    p.drawImage(0, 0, *src_image_);
    p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    p.end();
    need_calc_ = false;
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
    if(state == all_uncovered_)
        return;

    all_uncovered_ = state;
    merged_shape_ = QPainterPath();
    merged_shape_.setFillRule(Qt::WindingFill);
    foreach(auto sh, shapes_) {
        if(all_uncovered_) {
            if(uncovered_shapes_.contains(sh))
                uncovered_shapes_.remove(sh);
            sh->setOpacity(0.7f);
            sh->show();
            sh->setFlag(QGraphicsItem::ItemIsSelectable, true);
        }
        else {
            merged_shape_.addPath(sh->shape());
            sh->hide();
            sh->setFlag(QGraphicsItem::ItemIsSelectable, false);
        }
    }

    merged_shape_ = merged_shape_.simplified();
    merged_shape_.setFillRule(Qt::WindingFill);

    actions_["uncover"]->setEnabled(!all_uncovered_);
    scheduleCalcResultImage();
}

void InteractiveImage::finalizeShapeDraw()
{
    prepareGeometryChange();
    creating_shape_ = false;
    if(all_uncovered_) {
        foreach(auto sh, shapes_)
            sh->setFlag(QGraphicsItem::ItemIsSelectable, true);
    }

    QPainterPath p;
    p.addRect(shape_rect_);
    InteractiveImageShape* img_shape = new InteractiveImageShape(p);
    addShape(img_shape);
}

void InteractiveImage::unveilShapes(InteractiveImageToken *it)
{
    QSet<InteractiveImageShape*> unrevealed(shapes_.toSet());
    unrevealed.subtract(uncovered_shapes_);
    foreach(auto sh, unrevealed) {
        if(sh->shape().contains(it->pos())) {
            paths_.append(sh->shape());
            uncovered_shapes_.insert(sh);
            sh->hide();
        }
    }
}

const QRectF InteractiveImage::orderedRect(const QPointF &p1, const QPointF &p2)
{
    return QRectF(
        QPointF(
            qMin(p1.x(), p2.x()),
            qMin(p1.y(), p2.y())
        ),
        QPointF(
            qMax(p1.x(), p2.x()),
            qMax(p1.y(), p2.y())
        )
    );
}

void InteractiveImage::initContextMenu()
{
    context_menu_ = new QMenu;

    actions_["create_token"] = new QAction(tr("Create Token"));
    connect(actions_["create_token"], SIGNAL(triggered()),
            this, SLOT(onCreateToken()));

    actions_["create_shape"] = new QAction(tr("Create Shape"));
    connect(actions_["create_shape"], SIGNAL(triggered()),
            this, SLOT(onCreateShape()));

    actions_["merge_shapes"] = new QAction(tr("Merge Shapes"));
    connect(actions_["merge_shapes"], SIGNAL(triggered()),
            this, SLOT(onMergeShapes()));
    actions_["merge_shapes"]->setEnabled(false);

    actions_["cover"] = new QAction(tr("Overlay Map Fog"));
    connect(actions_["cover"], SIGNAL(triggered()),
            this, SLOT(onCoverAll()));

    actions_["uncover"] = new QAction(tr("Clear Map Fog"));
    connect(actions_["uncover"], SIGNAL(triggered()),
            this, SLOT(onUncoverAll()));
    actions_["uncover"]->setEnabled(!all_uncovered_);

    context_menu_->addAction(actions_["create_token"]);
    context_menu_->addAction(actions_["create_shape"]);
    context_menu_->addSeparator();
    context_menu_->addAction(actions_["merge_shapes"]);
    context_menu_->addSeparator();
    context_menu_->addAction(actions_["cover"]);
    context_menu_->addAction(actions_["uncover"]);
    context_menu_->addSeparator();
}
