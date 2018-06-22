#include "tuio_control_panel.h"

#include <QHostAddress>
#include <QJsonDocument>

#include <QHBoxLayout>

#include <QGraphicsView>
#include <QPainter>
#include <QtMath>
#include <QBrush>
#include <QMouseEvent>

#include "image/image_item.h"
#include "image/interactive/interactive_image.h"

TuioControlPanel::TuioControlPanel(QWidget *parent)
    : QWidget(parent)
    , marker_list_()
    , token_list_()
    , image_view_(0)
    , image_interactive_(false)
    , view_(0)
    , tuio_handler_(0)
{
    setWindowTitle("Tuio Control Panel");
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnBottomHint);

    tuio_handler_ = new TuioHandler(this);

    initWidgets();
    initLayout();

    connect(tuio_handler_, &TuioHandler::cursorEvent,
            this, &TuioControlPanel::onCursorEvent);
    connect(tuio_handler_, &TuioHandler::tokenEvent,
            this, &TuioControlPanel::onTokenEvent);
}


TuioControlPanel::~TuioControlPanel()
{
    for(auto m : marker_list_) {
        delete m;
    }
}

void TuioControlPanel::setImageView(Image::View *view)
{
    if(image_view_) {
        disconnect(image_view_, &Image::View::interactiveEnabled,
                   this, &TuioControlPanel::onImageInteractiveEnabled);
    }

    image_view_ = view;
    connect(image_view_, &Image::View::interactiveEnabled,
            this, &TuioControlPanel::onImageInteractiveEnabled);
}

void TuioControlPanel::onCursorEvent(QMap<int, QTuioCursor> active_cursors, QVector<QTuioCursor> dead_cursors)
{
    // add cursor
    for(auto id: active_cursors.keys())
    {
        QTuioCursor cursor = active_cursors[id];
        if(!marker_list_.keys().contains(id)) {
            auto marker = new QGraphicsEllipseItem(0,0,10,10);
            marker->setX(view_->sceneRect().width() * (1-cursor.x()));
            marker->setY(view_->sceneRect().height() * (1-cursor.y()));
            marker_list_[id] = marker;
            view_->scene()->addItem(marker);
        } else {
            marker_list_[id]->setX(view_->sceneRect().width() * (1-cursor.x()));
            marker_list_[id]->setY(view_->sceneRect().height() * (1-cursor.y()));
        }
    }

    // remove all dead cursors
    for(QTuioCursor c: dead_cursors)
    {
        int id = c.id();
        if(marker_list_.keys().contains(id)) {
            view_->scene()->removeItem(marker_list_[id]);
            delete marker_list_[id];
            marker_list_.remove(id);
        }
    }
}

void TuioControlPanel::onTokenEvent(QMap<int, QTuioToken> active_token, QVector<QTuioToken> dead_token)
{

    qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
    qDebug() << "  >" << "foo";

    for(int id: active_token.keys()) {

        QTuioToken token = active_token[id];
        if(!token_list_.keys().contains(id)) {
            auto marker = new QGraphicsRectItem(0,0,10,10);
            marker->setBrush(QBrush(Qt::red));
            marker->setX(view_->sceneRect().width() * (1-token.x()));
            marker->setY(view_->sceneRect().height()* (1-token.y()));
            marker->setRotation(qRadiansToDegrees(token.angle()));
            token_list_[id] = marker;
            view_->scene()->addItem(marker);
        } else {
            token_list_[id]->setX(view_->sceneRect().width() * (1-token.x()));
            token_list_[id]->setY(view_->sceneRect().height() * (1-token.y()));
            token_list_[id]->setRotation(qRadiansToDegrees(token.angle()));
        }
    }

    for(QTuioToken t: dead_token) {
        int id = t.id();
        if(token_list_.keys().contains(id)) {
            view_->scene()->removeItem(token_list_[id]);
            delete token_list_[id];
            token_list_.remove(id);
        }
    }

    if(image_interactive_)
        updateInteractiveImageTokens(active_token);
}

void TuioControlPanel::onImageInteractiveEnabled(bool enabled)
{
    image_interactive_ = enabled;
}

void TuioControlPanel::initWidgets()
{
    view_ =  new TuioGraphicsView(this);
    view_->setRenderHints(QPainter::Antialiasing);
}

void TuioControlPanel::initLayout()
{
    QHBoxLayout *root = new QHBoxLayout;
    root->setContentsMargins(0,0,0,0);
    root->addWidget(view_);

    setLayout(root);
}

void TuioControlPanel::updateInteractiveImageTokens(const QMap<int, QTuioToken> &active_token)
{
    if(active_token.size() == 0)
        return;

    QTuioToken token = active_token[active_token.keys()[0]];
    QGraphicsItem *it = image_view_->getItem();
    InteractiveImage* image_item = qgraphicsitem_cast<InteractiveImage*>(it);
    if(!image_item)
        return;

    QList<InteractiveImageToken*> iit = image_item->getTokens();
    if(iit.size() == 0)
        return;

    iit[0]->setX(image_view_->scene()->width() * (1 - token.x()));
    iit[0]->setY(image_view_->scene()->height() * (1 - token.y()));

}
