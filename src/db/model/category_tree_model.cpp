#include "category_tree_model.h"

#include <QDebug>
#include <QRegExp>

namespace DB {
namespace Model {

CategoryTreeModel::CategoryTreeModel(Core::Api* api, QObject* parent)
    : QStandardItemModel(parent)
    , table_model_(0)
    , api_(api)
    , categories_()
    , category_to_item_()
    , editable_(true)
{}

void CategoryTreeModel::select()
{
    table_model_ = api_->getCategoryTable();
    connect(table_model_, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(dbRowsInserted(QModelIndex,int,int)));
    connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(onDataChanged(QModelIndex,QModelIndex,QVector<int>)));

    if(table_model_)
        createModel();
}

void CategoryTreeModel::setEditable(bool editable)
{
    editable_ = editable;
}

Qt::ItemFlags CategoryTreeModel::flags(const QModelIndex &index) const
{
    if(editable_)
        return QStandardItemModel::flags(index);
    else
        return QStandardItemModel::flags(index) & (~Qt::ItemIsEditable);
}

CategoryRecord* CategoryTreeModel::getCategoryByItem(QStandardItem* item)
{
    if(item == 0)
        return 0;

    int rid = item->data(Qt::UserRole).toInt();
    return getCategoryById(rid);
}

CategoryRecord* CategoryTreeModel::getCategoryByPath(const QStringList& path)
{
    return getCategoryByItem(getItemByPath(path));
}

CategoryRecord* CategoryTreeModel::getCategoryById(int rid)
{
    if(categories_.contains(rid))
        return categories_[rid];
    else
        return 0;
}

CategoryRecord *CategoryTreeModel::getCategoryByIndex(const QModelIndex& index)
{
    if(!index.isValid())
        return 0;

    return getCategoryById(index.data(Qt::UserRole).toInt());
}

QStandardItem* CategoryTreeModel::getItemByCategory(CategoryRecord* category)
{
    if(category_to_item_.contains(category))
        return category_to_item_[category];
    else
        return 0;
}

QStandardItem* CategoryTreeModel::getItemByPath(const QStringList &path)
{
    QStandardItem* item = invisibleRootItem();
    QString name = "";
    bool found_item = true;
    foreach(QString cat, path) {
        found_item = false;
        for(int child = 0; child < item->rowCount(); ++child) {
            name = getCategoryByItem(item->child(child))->name;
            if(equalCategoryName(name, cat)) {
                item = item->child(child);
                found_item  = true;
                break;
            }
        }
        if(!found_item)
            break;
    }

    if(found_item)
        return item;

    return 0;
}

const QStringList CategoryTreeModel::getSubCategoryNamesByItem(QStandardItem* item)
{
    QStringList list;

    if(item == 0)
        item = invisibleRootItem();

    for(int child = 0; child < item->rowCount(); ++child)
        list.append(getCategoryByItem(item->child(child))->name);

    return list;
}

const QStringList CategoryTreeModel::getSubCategoryNamesByCategory(CategoryRecord* category)
{
    return getSubCategoryNamesByItem(getItemByCategory(category));
}

const QStringList CategoryTreeModel::getSubCategoryNamesByCategoryId(int id)
{
    return getSubCategoryNamesByItem(getItemByCategory(getCategoryById(id)));
}

const QList<int> CategoryTreeModel::getSubCategoryIdsByItem(QStandardItem* item)
{
    QList<int> list;

    if(item == 0)
        item = invisibleRootItem();

    for(int child = 0; child < item->rowCount(); ++child)
        list.append(getCategoryByItem(item->child(child))->id);

    return list;
}

const QList<int> CategoryTreeModel::getSubCategoryIdsByCategory(CategoryRecord* category)
{
    return getSubCategoryIdsByItem(getItemByCategory(category));
}

const QList<int> CategoryTreeModel::getSubCategoryIdsByCategoryId(int id)
{
    return getSubCategoryIdsByItem(getItemByCategory(getCategoryById(id)));
}

bool CategoryTreeModel::equalCategoryName(const QString &left, const QString &right)
{
    return left.split(QRegExp("-|\\s")).toSet() == right.split(QRegExp("-|\\s")).toSet();
}

bool CategoryTreeModel::exists(const QString &name, CategoryRecord* parent)
{
    foreach(QString n, getSubCategoryNamesByCategory(parent)) {
        if(equalCategoryName(n,name))
            return true;
    }

    return false;
}

void CategoryTreeModel::update()
{
    table_model_->select();
    createModel();
    emit updated();
}

void CategoryTreeModel::dbRowsInserted(QModelIndex, int, int)
{
    createModel();
}

void CategoryTreeModel::onDataChanged(const QModelIndex &topLeft, const QModelIndex&, const QVector<int>&)
{
    QMap<QString, int> attribute_col;
    attribute_col["id"] = -1;
    attribute_col["name"] = -1;
    for(int col = 0; col < table_model_->columnCount(); ++col) {
        QString header_name = table_model_->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString();
        attribute_col[header_name] = col;
    }

    QString name = topLeft.data(Qt::DisplayRole).toString();
    int rid = topLeft.data(Qt::UserRole).toInt();

    for(int row = 0; row < table_model_->rowCount(); ++row) {
        QModelIndex index = table_model_->index(row,attribute_col["id"]);
        if(index.data().toInt() == rid) {
            QModelIndex index = table_model_->index(row,attribute_col["name"]);
            table_model_->setData(index, name);
            table_model_->submit();
        }
    }
}

void CategoryTreeModel::createModel()
{
    QMap<QString, int> attribute_col;
    attribute_col["id"] = -1;
    attribute_col["name"] = -1;
    attribute_col["parent_id"] = -1;
    for(int col = 0; col < table_model_->columnCount(); ++col) {
        QString header_name = table_model_->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString();
        attribute_col[header_name] = col;
    }

    if(attribute_col["id"] == -1 || attribute_col["name"] == -1){
        qDebug() << "FAILURE: incomplete category table.";
        return;
    }

    category_to_item_.clear();

    for(QMap<int, CategoryRecord*>::iterator it = categories_.begin(); it != categories_.end(); ++it)
        delete it.value();

    categories_.clear();

    QList<CategoryRecord*> root_categories;

    for(int row = 0; row < table_model_->rowCount(); ++row) {
        QModelIndex index;
        CategoryRecord* category = new CategoryRecord;

        index = table_model_->index(row, attribute_col["id"]);
        category->id = table_model_->data(index).toInt();
        index = table_model_->index(row, attribute_col["name"]);
        category->name = table_model_->data(index).toString();
        index = table_model_->index(row, attribute_col["parent_id"]);
        category->parent_id = table_model_->data(index).toInt();

        categories_[category->id] = category;

        if(category->parent_id == 0)
            root_categories.append(categories_[category->id]);
    }

    for(QMap<int, CategoryRecord*>::iterator it = categories_.begin(); it != categories_.end(); ++it) {
        CategoryRecord* category = it.value();
        if(category->parent_id > 0) {
            category->parent = categories_[category->parent_id];
            category->parent->children.append(category);
        }
    }

    clear();
    setColumnCount(1);
    setHorizontalHeaderItem(0, new QStandardItem("Categories"));

    //root_index_ = invisibleRootItem()->index();

    QStandardItem* parentItem = invisibleRootItem();
    setChildItems(parentItem, root_categories);
}

void CategoryTreeModel::setChildItems(QStandardItem* item, QList<CategoryRecord*> children)
{
    for(int row = 0; row < children.size(); ++row) {
        QStandardItem* child_item = new QStandardItem;
        child_item->setData(children[row]->id, Qt::UserRole);
        child_item->setData(children[row]->name, Qt::DisplayRole);
        item->setChild(row,0,child_item);

        category_to_item_[children[row]] = child_item;

        if(children[row]->children.size() > 0) {
            setChildItems(child_item, children[row]->children);
        }
    }
}

} // namespace Model
} // namespace DB
