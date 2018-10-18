#ifndef CATEGORY_TREE_VIEW_H
#define CATEGORY_TREE_VIEW_H

#include <QTreeView>

class QMouseEvent;
struct CategoryRecord;
namespace DB {
    namespace Model {
        class CategoryTreeModel;
    }
}

class CategoryTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit CategoryTreeView(QWidget *parent = 0);

    void setCategoryTreeModel(DB::Model::CategoryTreeModel* model);

    void mousePressEvent(QMouseEvent *event);

signals:
    void categorySelected(CategoryRecord* rec);

public slots:
    void selectRoot();

private slots:
    void onClicked(QModelIndex const&);

private:
    DB::Model::CategoryTreeModel* model_;

};

#endif // TREEVIEW_H
