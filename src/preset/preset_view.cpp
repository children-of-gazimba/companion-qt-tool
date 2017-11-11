#include "preset_view.h"

#include <QDrag>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMimeData>
#include <QApplication>
#include <QDebug>

#include "resources/lib.h"

namespace Preset {

PresetView::PresetView(QWidget *parent)
    : QListView(parent)
    , start_pos_()
    , model_(0)
{
    model_ = new Misc::StandardItemModel(this);
    model_->setColumnCount(1);
    model_->setHorizontalHeaderItem(0, new QStandardItem("Name"));
    model_->setHorizontalHeaderItem(1, new QStandardItem("Path"));
    model_->setRowCount(1);
    model_->setData(model_->index(0,0), QVariant("New Preset"));
    model_->setData(model_->index(0,1), QVariant("C:\\Users\\Basti\\Documents\\Code\\pap-media\\pap-media-shared-files\\empty_preset.json"));
    setModel(model_);
    //setAcceptDrops(false);
    setEditable(false);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
}

PresetView::~PresetView()
{}

void PresetView::setEditable(bool is_editable)
{
    for(int i = 0; i < model_->columnCount(); ++i)
        model_->setColumnEditable(i, is_editable);
}

bool PresetView::getEditable()
{
    return model_->getColumnEditable(0);
}

void PresetView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        start_pos_ = event->pos();
    }
    QListView::mousePressEvent(event);
}

void PresetView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        int distance = (event->pos() - start_pos_).manhattanLength();
        if (distance >= QApplication::startDragDistance()) {
            performDrag();
        }
    }
    QListView::mouseMoveEvent(event);
}

void PresetView::dragEnterEvent(QDragEnterEvent *event)
{
    event->ignore();
}

void PresetView::dragMoveEvent(QDragMoveEvent *event)
{
    event->ignore();
}

void PresetView::dropEvent(QDropEvent *event)
{
    event->ignore();
}

void PresetView::performDrag()
{
    if(selectionModel()->selectedRows().size() == 1) {
        QModelIndex idx = selectionModel()->selectedRows(1)[0];

        QString f_name = model()->data(idx).toString();
        QFile f(f_name);
        if(!f.open(QFile::ReadOnly)) {
            qDebug() << "could not open file";
        }
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        f.close();

        QJsonObject obj;
        obj["name"] =  f_name;
        obj["size"] = 1;
        obj["contents"] = doc.object();
        QJsonArray pos_arr;
        pos_arr.append(300);
        pos_arr.append(150);
        obj["position"] = pos_arr;

        doc.setObject(obj);

        QMimeData* mime_data = new QMimeData;
        mime_data->setText(QString(doc.toJson()));

        // create Drag
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mime_data);
        drag->setPixmap(*Resources::Lib::PX_SOUND_FILE_DRAG);

        // will block until drag done
        drag->exec(Qt::CopyAction);
    }
}

} // namespace Preset
