#include "category_tree_view.h"

#include <QTreeView>
#include <QMouseEvent>

#include "db/model/category_tree_model.h"

CategoryTreeView::CategoryTreeView(QWidget *parent)
    : QTreeView(parent)
    , model_(0)
{
    connect(this, SIGNAL(clicked(const QModelIndex&)),
            this, SLOT(onClicked(const QModelIndex&)));
}

void CategoryTreeView::setCategoryTreeModel(DB::Model::CategoryTreeModel *model)
{
    model_ = model;
    setModel(model);
    selectRoot();
}

void CategoryTreeView::mousePressEvent(QMouseEvent *event)
{
    QTreeView::mousePressEvent(event);

    QModelIndex item = indexAt(event->pos());
    if(item.row() == -1 && item.column() == -1) {
        selectRoot();
    }
}

void CategoryTreeView::selectRoot()
{
    selectionModel()->clearSelection();
    emit categorySelected(0);
}

void CategoryTreeView::onClicked(const QModelIndex& index)
{
    if(model_ == 0)
        return;
    else if(!index.isValid())
        selectRoot();
    else
        emit categorySelected(model_->getCategoryByIndex(index));
}
