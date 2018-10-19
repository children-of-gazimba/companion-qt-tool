#ifndef CATEGORY_TREE_VIEW_H
#define CATEGORY_TREE_VIEW_H

#include <QTreeView>

class QMouseEvent;
struct CategoryRecord;
class CategoryTreeModel;

class CategoryTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit CategoryTreeView(QWidget *parent = 0);

    void setCategoryTreeModel(CategoryTreeModel* model);

    void mousePressEvent(QMouseEvent *event);

signals:
    void categorySelected(CategoryRecord* rec);

public slots:
    void selectRoot();

private slots:
    void onClicked(QModelIndex const&);

private:
    CategoryTreeModel* model_;
};

#endif // CATEGORY_TREE_VIEW_H
