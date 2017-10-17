#include "file_table.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QFileInfoList>
#include <QDir>
#include <QString>
#include <QHeaderView>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>

namespace Image {

FileTable::FileTable(QWidget *parent)
    : QWidget(parent)
    , model_(0)
    , file_view_(0)
    , image_view_(0)
    , open_button_(0)
{
    initWidgets();
    initLayout();
}

void FileTable::onOpen()
{
    QString dir_name = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
    if(dir_name.size() == 0) {
        qDebug() << "nothing opened";
        return;
    }
    QDir dir(dir_name);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.bmp";
    QFileInfoList image_files = dir.entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot);
    model_->removeRows(0, model_->rowCount());
    model_->setRowCount(image_files.size());
    int i = 0;
    foreach (QFileInfo info, image_files) {
        QString path = info.absoluteFilePath();
        QString name = info.fileName();
        model_->setData(model_->index(i,0), QVariant(name));
        model_->setData(model_->index(i,0), QVariant("<html><div><img src=\""+path+"\" style=\"width:100%;\" height=\"200\" /></div></html>"), Qt::ToolTipRole);
        model_->setData(model_->index(i,1), QVariant(path));
        model_->setData(model_->index(i,1), QVariant("<html><div><img src=\""+path+"\" style=\"width:100%;\" height=\"200\" /></div></html>"), Qt::ToolTipRole);
        ++i;
    }
    file_view_->resizeColumnsToContents();
}

void FileTable::onImageSelected(const QModelIndex& idx)
{
    if(!idx.isValid())
        return;
    onImageSelected(idx.row());
}

void FileTable::onImageSelected(int row)
{
    if(row < 0 || row >= model_->rowCount())
        return;
    QString path = model_->data(model_->index(row, 1)).toString();
    image_view_->clear();
    image_view_->setItem(new QGraphicsPixmapItem(QPixmap(path)));
    image_view_->show();
    image_view_->activateWindow();
}

void FileTable::initWidgets()
{
    model_ = new QStandardItemModel(0, 2, this);
    model_->setHeaderData(0, Qt::Horizontal, QVariant("Name"));
    model_->setHeaderData(1, Qt::Horizontal, QVariant("Path"));

    file_view_ = new QTableView(this);
    file_view_->setModel(model_);
    file_view_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    file_view_->setSelectionBehavior(QAbstractItemView::SelectRows);
    file_view_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    file_view_->verticalHeader()->hide();

    connect(file_view_, SIGNAL(clicked(const QModelIndex&)),
            this, SLOT(onImageSelected(const QModelIndex&)));
    connect(file_view_->verticalHeader(), SIGNAL(sectionClicked(int)),
            this, SLOT(onImageSelected(int)));

    image_view_ = new View;
    image_view_->setWindowFlags(Qt::Window);
    image_view_->hide();

    open_button_ = new QPushButton(tr("browse"), this);
    connect(open_button_, SIGNAL(clicked()),
            this, SLOT(onOpen()));
}

void FileTable::initLayout()
{
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(file_view_, 5);
    layout->addWidget(open_button_, 0);
    setLayout(layout);
}

} // namespace Image
