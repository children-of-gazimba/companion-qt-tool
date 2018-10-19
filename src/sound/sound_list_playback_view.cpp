#include "sound_list_playback_view.h"

#include <QDebug>
#include <QMimeData>
#include <QApplication>
#include <QDrag>
#include <QPixmap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHeaderView>

#include "resources/lib.h"
#include "misc/json_mime_data_parser.h"

SoundListPlaybackView::SoundListPlaybackView(const QList<SoundFileRecord *> &sound_files, QWidget *parent)
    : QTableView(parent)
    , start_pos_()
    , model_(0)
    , skip_select_(false)
    , playable_index_()
    , play_icon_()
    , context_menu_(0)
{
    init();
    setSoundFiles(sound_files);
}

SoundListPlaybackView::SoundListPlaybackView(QWidget *parent)
    : QTableView(parent)
    , start_pos_()
    , model_(0)
    , skip_select_(false)
    , playable_index_()
    , play_icon_()
    , context_menu_(0)
{
    init();
}

SoundListPlaybackView::~SoundListPlaybackView()
{}

void SoundListPlaybackView::setSoundFiles(const QList<SoundFileRecord *> &sound_files)
{
    model_->clear();
    foreach(SoundFileRecord* rec, sound_files)
        addSoundFile(rec);
    setColumnWidth(0, verticalHeader()->defaultSectionSize());
}

void SoundListPlaybackView::setEditable(bool is_editable)
{
    for(int i = 0; i < model_->columnCount(); ++i)
        model_->setColumnEditable(i, is_editable);
}

bool SoundListPlaybackView::getEditable()
{
    return model_->getColumnEditable(0);
}

QItemSelectionModel::SelectionFlags SoundListPlaybackView::selectionCommand(const QModelIndex &index, const QEvent *event) const
{
    if (event != 0 && event->type() == QEvent::MouseMove)
        return QItemSelectionModel::Select;
    else
        return QAbstractItemView::selectionCommand(index, event);
}

void SoundListPlaybackView::mousePressEvent(QMouseEvent *event)
{
    if(state() == QAbstractItemView::DragSelectingState)
        setState(QAbstractItemView::NoState);
    QModelIndex idx = indexAt(event->pos());
    if(idx.row() == -1 && idx.column() == -1)
        selectionModel()->clear();
    if (event->button() == Qt::LeftButton)
        start_pos_ = event->pos();
    QTableView::mousePressEvent(event);
}

void SoundListPlaybackView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        int distance = (event->pos() - start_pos_).manhattanLength();
        if (distance >= QApplication::startDragDistance()) {
            performDrag();
            return;
        }
    }
    QTableView::mouseMoveEvent(event);
}

void SoundListPlaybackView::mouseReleaseEvent(QMouseEvent *event)
{
    if(state() == QAbstractItemView::DragSelectingState)
        setState(QAbstractItemView::NoState);
    QTableView::mouseReleaseEvent(event);
}

void SoundListPlaybackView::dragEnterEvent(QDragEnterEvent *event)
{
    auto source = qobject_cast<SoundListPlaybackView*>(event->source());
    if (source && source != this) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void SoundListPlaybackView::dragMoveEvent(QDragMoveEvent *event)
{
    auto source = qobject_cast<SoundListPlaybackView*>(event->source());
    if (source && source != this) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void SoundListPlaybackView::dropEvent(QDropEvent *event)
{
    auto source = qobject_cast<SoundListPlaybackView*>(event->source());
    if (source && source != this) {
        // extract TableRecord from mime data
        QList<TableRecord*> records = JsonMimeDataParser::toTableRecordList(event->mimeData());

        // validate parsing
        if(records.size() == 0) {
            event->ignore();
            return;
        }

        // handle extracted data
        foreach(TableRecord* rec, records) {
            if(rec->index == SOUND_FILE) {
                SoundFileRecord* sound_rec = (SoundFileRecord*) rec;
                addSoundFile(sound_rec->id, sound_rec->name, sound_rec->path);
            }
        }

        // delete temp records
        while(records.size() > 0) {
            delete records[0];
            records.pop_front();
        }

        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void SoundListPlaybackView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    if(skip_select_) {
        skip_select_ = false;
        selectionModel()->clearSelection();
        setSelection(rect, QItemSelectionModel::Deselect);
        return;
    }
    QTableView::setSelection(rect, command);
}

void SoundListPlaybackView::onPlayButtonClicked()
{
    if(!playable_index_.isValid())
        return;

    SoundFileRecord rec;
    rec.id = model_->data(model_->index(playable_index_.row(), 0), Qt::UserRole).toInt();
    rec.path = model_->data(model_->index(playable_index_.row(), 0), Qt::UserRole+1).toString();
    rec.name = model_->data(model_->index(playable_index_.row(), 1)).toString();
    emit play(rec);
}

void SoundListPlaybackView::addSoundFile(SoundFileRecord *rec)
{
     addSoundFile(rec->id, rec->name, rec->path);
}

void SoundListPlaybackView::onSoundFileAboutToBeDeleted(SoundFileRecord *)
{
    qDebug() << "TODO implement sound file about to be deleted.";
}

void SoundListPlaybackView::onDropSuccessful()
{
    QCoreApplication::processEvents();
    skip_select_ = true;
}

void SoundListPlaybackView::addSoundFile(int id, const QString &name, const QString &path)
{
    QList<QStandardItem*> items;
    items.push_back(new QStandardItem(path));
    items.push_back(new QStandardItem(name));
    model_->appendRow(items);
    QModelIndex idx = model_->index(model_->rowCount()-1, 0);
    model_->setData(idx, QVariant(id), Qt::UserRole);
    model_->setData(idx, QVariant(path), Qt::UserRole+1);
    model_->setData(idx, QVariant(""), Qt::DisplayRole);
}

void SoundListPlaybackView::onEntered(const QModelIndex &idx)
{
    if(idx.isValid()) {
        QModelIndex b_idx = model_->index(idx.row(), 0);
        auto button = new QPushButton;
        button->setIcon(play_icon_);
        button->setStyleSheet(
            "QPushButton:hover {"
            "   background-color: rgba(54, 86, 117, 255);"
            "   color: white;"
            "}"
        );
        connect(button, &QPushButton::clicked,
                this, &SoundListPlaybackView::onPlayButtonClicked);
        setIndexWidget(b_idx, button);
        if(playable_index_.isValid() && playable_index_ != b_idx)
            setIndexWidget(playable_index_, 0);
        playable_index_ = QPersistentModelIndex(b_idx);
    }
}

void SoundListPlaybackView::showCustomContextMenu(const QPoint &p)
{
    context_menu_->exec(mapToGlobal(p));
}

void SoundListPlaybackView::onDeleteAction()
{
    QModelIndexList selection = this->selectionModel()->selectedIndexes();
    if(selection.size() == 0)
        return;

    int row = selection.first().row();
    int id = model_->data(model_->index(row, 0), Qt::UserRole).toInt();
    model_->removeRow(row);
    emit deleteSoundFileRequested(id);
}

void SoundListPlaybackView::performDrag()
{
    QList<TableRecord*> records;
    QSet<int> rows;
    foreach(QModelIndex idx, selectionModel()->selectedIndexes()) {
        if(rows.contains(idx.row()))
            continue;
        SoundFileRecord* temp_rec = new SoundFileRecord;
        temp_rec->id = model_->data(model_->index(idx.row(), 0), Qt::UserRole).toInt();
        temp_rec->path = model_->data(model_->index(idx.row(), 0), Qt::UserRole+1).toString();
        temp_rec->name = model_->data(model_->index(idx.row(), 1)).toString();
        records.append(temp_rec);
        rows.insert(idx.row());
    }

    if(records.size() == 0)
        return;

    selectionModel()->clear();

    // create QMimeData
    QMimeData* mime_data = JsonMimeDataParser::toJsonMimeData(records);

    // delete temporary TableRecords
    while(records.size() > 0) {
        delete records[0];
        records.pop_front();
    }

    // create Drag
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mime_data);
    drag->setPixmap(*Resources::Lib::PX_SOUND_FILE_DRAG);

    // will block until drag done
    drag->exec(Qt::CopyAction);
}

void SoundListPlaybackView::initContextMenu()
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    context_menu_ = new QMenu(this);

    connect(this, &SoundListPlaybackView::customContextMenuRequested,
            this, &SoundListPlaybackView::showCustomContextMenu);

    QList<QAction*> actions;
    actions.append(new QAction(tr("Delete"), context_menu_));
    connect(actions.back(), SIGNAL(triggered()),
            this, SLOT(onDeleteAction()));

    context_menu_->addActions(actions);
}

void SoundListPlaybackView::init()
{
    model_ = new StandardItemModel(this);
    model_->setColumnCount(2);
    model_->setHorizontalHeaderItem(0, new QStandardItem("Path"));
    model_->setHorizontalHeaderItem(1, new QStandardItem("Name"));

    play_icon_ = QIcon(*Resources::Lib::PX_PLAY);

    setMouseTracking(true);
    connect(this, &SoundListPlaybackView::entered,
            this, &SoundListPlaybackView::onEntered);

    setModel(model_);
    setAcceptDrops(false);
    setEditable(false);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setColumnWidth(0, verticalHeader()->defaultSectionSize());
    setShowGrid(false);

    verticalHeader()->hide();
    horizontalHeader()->hide();
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    horizontalHeader()->setStretchLastSection(true);

    initContextMenu();
}
