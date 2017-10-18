#include "full_browser.h"

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

FullBrowser::FullBrowser(QWidget *parent)
    : QWidget(parent)
    , recent_directories_(0)
    , list_view_(0)
    , v_splitter_(0)
{
    initWidgets();
    initLayout();
}

void FullBrowser::onNewDirectory(const QString & name)
{
    QStandardItemModel* m = (QStandardItemModel*) recent_directories_->model();
    for(int i = 0; i < m->rowCount(); ++i) {
        QString dir_name = m->data(m->index(i, 0)).toString();
        if(name.compare(dir_name) == 0)
            return;
    }
    m->appendRow(new QStandardItem(name));
}

void FullBrowser::onRecentDirClicked(const QModelIndex& idx)
{
    if(!idx.isValid() || idx.row() < 0)
        return;
    QStandardItemModel* m = (QStandardItemModel*) recent_directories_->model();
    QString dir =  m->data(idx).toString();
    list_view_->openDirectory(dir);
}

void FullBrowser::initWidgets()
{
    v_splitter_ = new QSplitter(Qt::Vertical, this);

    QStandardItemModel* model = new QStandardItemModel(0, 1, this);
    model->setHeaderData(0, Qt::Horizontal, tr("Recent Directories"));
    recent_directories_ = new QTreeView(v_splitter_);
    recent_directories_->setModel(model);

    connect(recent_directories_, SIGNAL(clicked(const QModelIndex&)),
            this, SLOT(onRecentDirClicked(const QModelIndex&)));

    list_view_ = new ListBrowser(v_splitter_);
    list_view_->layout()->setMargin(0);

    connect(list_view_, SIGNAL(newDirectory(const QString&)),
            this, SLOT(onNewDirectory(const QString&)));

    v_splitter_->addWidget(recent_directories_);
    v_splitter_->addWidget(list_view_);
    v_splitter_->setStretchFactor(0, 2);
    v_splitter_->setStretchFactor(1, 8);
}

void FullBrowser::initLayout()
{
    QVBoxLayout* layout = new QVBoxLayout;

    layout->addWidget(v_splitter_);

    setLayout(layout);
}

} // namespace Image
