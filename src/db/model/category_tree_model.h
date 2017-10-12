#ifndef DB_MODEL_CATEGORY_TREE_MODEL_H
#define DB_MODEL_CATEGORY_TREE_MODEL_H

#include <QStandardItemModel>
#include <QSqlRelationalTableModel>

#include "db/table_records.h"
#include "db/core/api.h"

namespace DB {
namespace Model {

/*
 * Class derived from QStandartItemModel
 * contains an instance of QSqlRelationalTableModel
 * which refers to a hierarchical category db table.
 * This class will recursively transfer given table into
 * an n-dimensional QStandardItemModel, which can be connected
 * to a QTreeView. Convinience functions ease data access.
*/

class CategoryTreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    CategoryTreeModel(Core::Api* api, QObject* parent = 0);

    /* Fills model with data from Category database table **/
    void select();

    /* Sets whether the content of this model can be edited **/
    void setEditable(bool);

    //// inheritted functions (from pure virtual BC) - see docs for description
    Qt::ItemFlags flags(const QModelIndex &index) const;
    //// end inheritted functions

    /*
     * Gets a CategoryRecord based on the QStandardItem referencing it.
     * returns 0 if none found.
    */
    CategoryRecord* getCategoryByItem(QStandardItem*);

    /*
     * Gets a CategoryRecord based on it's hierarchical path.
     * returns 0 if none found.
    */
    CategoryRecord* getCategoryByPath(QStringList const&);

    /*
     * Gets a CategoryRecord based on it's ID.
     * returns 0 if none found.
    */
    CategoryRecord* getCategoryById(int);

    /*
     * Gets a CategoryRecord based on it's QModelIndex.
     * returns 0 if none found.
    */
    CategoryRecord* getCategoryByIndex(QModelIndex const&);

    /*
     * Gets the QStandardItem associated with given CategoryRecord.
     * returns 0 if none found.
    */
    QStandardItem* getItemByCategory(CategoryRecord*);

    /*
     * Gets the QStandardItem associated with given
     * hierarchical CategoryRecord path.
     * returns 0 if none found.
    */
    QStandardItem* getItemByPath(QStringList const&);

    /*
     * Gets all subcategory names of CategoryRecord
     * referenced by given QStandardItem.
    */
    QStringList const getSubCategoryNamesByItem(QStandardItem* item = 0);

    /* Gets all subcategory names of given CategoryRecord */
    QStringList const getSubCategoryNamesByCategory(CategoryRecord*);

    /* Gets all subcategory ids of given CategoryRecord */
    QStringList const getSubCategoryNamesByCategoryId(int);

    /* Gets all subcategory ids of given CategoryRecord */
    QList<int> const getSubCategoryIdsByItem(QStandardItem* item = 0);

    /* Gets all subcategory ids of given CategoryRecord */
    QList<int> const getSubCategoryIdsByCategory(CategoryRecord*);

    /* Gets all subcategory ids of given CategoryRecord */
    QList<int> const getSubCategoryIdsByCategoryId(int);

    /* Adds a CategoryRecord to this model and  **/

    /*
     * Compares given Category names.
     * returns if both Categories have equal names.
     * (' ' and '-' are treated as same charachter)
    **/
    static bool equalCategoryName(QString const& left, QString const& right);

    /* Returns true if there exists a CategoryRecord with given name and parent CategoryRecord. **/
    bool exists(QString const& name, CategoryRecord* parent);

public slots:
    /* Reselects and builds the CategoryTreeModel **/
    void update();
    void dbRowsInserted(QModelIndex,int, int);
    void onDataChanged(QModelIndex const& topLeft,
                       QModelIndex const& bottomRight,
                       QVector<int> const& roles);

signals:
    void updated();

private:
    /* build this model based on underlying QSqlrelationalTableModel. **/
    void createModel();

    /*
     * Recursive function setting all child QStandardItems for given QStandardItem.
     * Parameter item references a CategoryRecord, with parameter children list of
     * child CategoryRecords.
    **/
    void setChildItems(QStandardItem* item, QList<CategoryRecord*> children);

    // underlying QSqlrelationalTableModel referencing DB Category table.
    QSqlRelationalTableModel* table_model_;

    Core::Api* api_;

    QMap<int, CategoryRecord*> categories_;
    QMap<CategoryRecord*, QStandardItem*> category_to_item_;

    bool editable_;
};

} // namespace Model
} // namespace DB
#endif // DB_MODEL_CATEGORY_TREE_MODEL_H
