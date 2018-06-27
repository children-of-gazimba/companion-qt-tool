#include "tuio_control_panel.h"

#include <QJsonDocument>

#include <QHBoxLayout>

#include <QGraphicsView>
#include <QPainter>
#include <QtMath>
#include <QBrush>
#include <QMouseEvent>
#include <QMessageBox>
#include <QHeaderView>
#include <QSplitter>
#include <QGroupBox>

#include "image/image_item.h"
#include "image/interactive/interactive_image.h"
#include "tuio_token_item.h"
#include "tuio_cursor_item.h"

TuioControlPanel::TuioControlPanel(QWidget *parent)
    : QWidget(parent)
    , marker_list_()
    , token_list_()
    , image_view_(0)
    , image_interactive_(false)
    , host_name_(0)
    , host_submit_(0)
    , host_label_(0)
    , view_(0)
    , tuio_handler_(0)
    , cursor_table_(0)
    , token_table_(0)
    , tracking_token_id_(-1)
    , tracking_token_class_id_(-1)
{
    setWindowTitle("Tuio Control Panel");
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);

    initTuio(QHostAddress::Any, 3333);
    initWidgets();
    initLayout();
}


TuioControlPanel::~TuioControlPanel()
{
    for(auto m : marker_list_.values()) {
        view_->scene()->removeItem(m);
        delete m;
    }
    for(auto t : token_list_.values()) {
        view_->scene()->removeItem(t);
        delete t;
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

void TuioControlPanel::onImageInteractiveEnabled(bool enabled)
{
    image_interactive_ = enabled;
}

void TuioControlPanel::onNewHostName()
{
    QList<QString> host_str(host_name_->text().split(":"));
    if(host_str.size() != 2) {
        QMessageBox b;
        b.setText(tr("Invalid address for TUIO server"));
        b.setInformativeText(tr("Address should be of format <xxx.xxx.xxx.xxx:port>"));
        b.setStandardButtons(QMessageBox::Ok);
        b.setDefaultButton(QMessageBox::Ok);
        b.exec();
        return;
    }

    QHostAddress addr(host_str[0]);
    unsigned port = (unsigned) host_str[1].toInt();
    initTuio(addr, port);

    QMessageBox b;
    b.setText(tr("New TUIO host set"));
    b.setInformativeText(addr.toString() + ":" + QString::number(port));
    b.setStandardButtons(QMessageBox::Ok);
    b.setDefaultButton(QMessageBox::Ok);
    b.exec();
}

void TuioControlPanel::onCursorChanged(int id, TuioCursorTableModel::CursorChange c)
{
    if(c == TuioCursorTableModel::CURSOR_ADDED || c == TuioCursorTableModel::CURSOR_UPDATED) {
        // add or update cursor marker
        QTuioCursor cursor = tuio_handler_->getCursorModel()->getCursor(id);
        if(cursor.id() == -1)
            return;
        if(!marker_list_.contains(id)) {
            auto marker = new TuioCursorItem(cursor.id());
            marker->setX(view_->sceneRect().width() * (1-cursor.x()));
            marker->setY(view_->sceneRect().height() * (1-cursor.y()));
            marker_list_[id] = marker;
            view_->scene()->addItem(marker);
        } else {
            marker_list_[id]->setX(view_->sceneRect().width() * (1-cursor.x()));
            marker_list_[id]->setY(view_->sceneRect().height() * (1-cursor.y()));
        }
    }
    else if(c == TuioCursorTableModel::CURSOR_REMOVED) {
        // delete cursor marker
        if(marker_list_.contains(id)) {
            view_->scene()->removeItem(marker_list_[id]);
            delete marker_list_[id];
            marker_list_.remove(id);
        }
    }
}

void TuioControlPanel::onTokenChanged(int id, TuioTokenTableModel::TokenChange c)
{
    if(c == TuioTokenTableModel::TOKEN_ADDED || c == TuioTokenTableModel::TOKEN_UPDATED) {
        // add or update token marker
        QTuioToken token = tuio_handler_->getTokenModel()->getToken(id);
        if(token.id() == -1)
            return;
        if(!token_list_.contains(id)) {
            auto marker = new TuioTokenItem(token.id(), token.classId());
            //marker->setBrush(QBrush(Qt::red));
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
        // TODO: improve
        if(image_interactive_ && isTrackingToken(token))
            updateInteractiveImageToken(token);
    }
    else if(c == TuioTokenTableModel::TOKEN_REMOVED) {
        // delete token marker
        if(token_list_.contains(id)) {
            view_->scene()->removeItem(token_list_[id]);
            delete token_list_[id];
            token_list_.remove(id);
        }
        if(id == tracking_token_id_)
            tracking_token_id_ = -1;
    }
}

void TuioControlPanel::onTokenFieldSelected(const QModelIndex &idx)
{
    QAbstractTableModel* m = tuio_handler_->getTokenModel();
    if(idx.row() < 0 || idx.row() > m->rowCount()) {
        resetTokenTracking();
    }
    else {
        int id_value = m->data(idx).toInt();
        switch(idx.column()) {
            case 0: setTrackingTokenID(id_value); break;
            case 1: setTrackingTokenClassID(id_value); break;
            default: resetTokenTracking(); break;
        }
    }
}

void TuioControlPanel::setTrackingTokenID(int id)
{
    tracking_token_class_id_ = -1;
    tracking_token_id_ = id;
}

void TuioControlPanel::setTrackingTokenClassID(int id)
{
    tracking_token_class_id_ = id;
    tracking_token_id_ = -1;
}

void TuioControlPanel::resetTokenTracking()
{
    tracking_token_id_ = -1;
    tracking_token_class_id_ = -1;
}

bool TuioControlPanel::isTrackingToken(const QTuioToken &t) const
{
    if(t.id() == -1)
        return false;
    return t.id() == tracking_token_id_ || t.classId() == tracking_token_class_id_;
}

void TuioControlPanel::initTuio(const QHostAddress &ip, unsigned port)
{
    if(tuio_handler_)
        tuio_handler_->deleteLater();
    tuio_handler_ = new TuioModelHandler(ip, port, this);
    connect(tuio_handler_->getCursorModel(), &TuioCursorTableModel::cursorChanged,
            this, &TuioControlPanel::onCursorChanged);
    connect(tuio_handler_->getTokenModel(), &TuioTokenTableModel::tokenChanged,
            this, &TuioControlPanel::onTokenChanged);
    if(cursor_table_)
        cursor_table_->setModel(tuio_handler_->getCursorModel());
    if(token_table_)
        token_table_->setModel(tuio_handler_->getTokenModel());
}

void TuioControlPanel::initWidgets()
{
    view_ =  new TuioGraphicsView(this);
    view_->setRenderHints(QPainter::Antialiasing);

    host_name_ = new QLineEdit(this);
    host_name_->setPlaceholderText("255.255.255.255:3333");

    host_submit_ = new QPushButton(tr("Submit"), this);
    connect(host_submit_, &QPushButton::clicked,
            this, &TuioControlPanel::onNewHostName);

    host_label_ = new QLabel(tr("TUIO Host"), this);

    cursor_table_ = new QTableView(this);
    cursor_table_->setModel(tuio_handler_->getCursorModel());
    cursor_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    cursor_table_->horizontalHeader()->setStretchLastSection(true);
    cursor_table_->setDragEnabled(true);
    cursor_table_->horizontalHeader()->setSectionsMovable(true);

    token_table_ = new QTableView(this);
    token_table_->setModel(tuio_handler_->getTokenModel());
    token_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    token_table_->horizontalHeader()->setStretchLastSection(true);
    token_table_->setDragEnabled(true);
    token_table_->horizontalHeader()->setSectionsMovable(true);
    connect(token_table_, &QTableView::clicked,
            this, &TuioControlPanel::onTokenFieldSelected);
}

void TuioControlPanel::initLayout()
{
    QHBoxLayout* bottom = new QHBoxLayout;
    bottom->addWidget(host_label_,-1);
    bottom->addWidget(host_name_, 100);
    bottom->addWidget(host_submit_,-1);
    bottom->setContentsMargins(5, 5, 5, 5);
    bottom->setSpacing(5);

    QSplitter* top_splitter = new QSplitter(Qt::Horizontal, this);

    QSplitter* monitor_splitter = new QSplitter(Qt::Vertical, this);
    QGroupBox* token_box = new QGroupBox(tr("Token Monitor"), this);
    token_box->setLayout(new QHBoxLayout);
    token_box->layout()->addWidget(token_table_);
    QGroupBox* cursor_box = new QGroupBox(tr("Cursor Monitor"), this);
    cursor_box->setLayout(new QHBoxLayout);
    cursor_box->layout()->addWidget(cursor_table_);
    monitor_splitter->addWidget(token_box);
    monitor_splitter->addWidget(cursor_box);

    QGroupBox* box_left = new QGroupBox(this);
    box_left->setLayout(new QHBoxLayout);
    box_left->layout()->addWidget(monitor_splitter);

    QGroupBox* box_right = new QGroupBox(this);
    box_right->setLayout(new QHBoxLayout);
    box_right->layout()->addWidget(view_);

    top_splitter->addWidget(box_left);
    top_splitter->addWidget(box_right);
    top_splitter->setStretchFactor(0, 1);
    top_splitter->setStretchFactor(0, 3);
    top_splitter->setContentsMargins(5,0,5,0);

    QVBoxLayout *root = new QVBoxLayout;
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);
    root->addWidget(top_splitter);
    root->addLayout(bottom,-1);

    setLayout(root);
}

void TuioControlPanel::updateInteractiveImageToken(const QTuioToken &token)
{
    QGraphicsItem *it = image_view_->getItem();
    InteractiveImage* image_item = qgraphicsitem_cast<InteractiveImage*>(it);
    if(!image_item)
        return;

    QList<InteractiveImageToken*> iit = image_item->getTokens();
    if(iit.size() == 0)
        return;

    QPointF new_pos;
    new_pos.setX(image_view_->scene()->width() * (1 - token.x()));
    new_pos.setY(image_view_->scene()->height() * (1 - token.y()));
    iit[0]->setUncoverPos(new_pos);
}
