#ifndef DB_MODEL_TAG_TABLE_MODEL_H
#define DB_MODEL_TAG_TABLE_MODEL_H

#include <QAbstractTableModel>
#include "db/core/api.h"
#include "db/table_records.h"

namespace DB {
namespace Model {

class TagTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TagTableModel(Core::Api* api, QObject *parent = 0);
    ~TagTableModel();

    //// inheritted functions (from pure virtual BC) - see docs for description

    // read
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    //write
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::DisplayRole);

    /* does nothing, returns false */
    bool removeColumn(int column, const QModelIndex &parent = QModelIndex());

    /* does nothing, returns false */
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());

    bool removeRow(int row, const QModelIndex &parent = QModelIndex());

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    //// end inheritted functions

    /* Update model with data from db */
    void update();

    /* Fills model with data from SoundFile database table **/
    void select();

    /*
     * Gets the row of TagRecord.
     * Returns -1 if none found
    */
    int getRowByTag(DB::TagRecord* rec);

    /*
     * Gets TagRecord based on ID.
     * Returns 0 if none found.
    */
    TagRecord* getTagById(int id);

    /*
     * Gets TagRecord based on name.
     * Returns 0 if none found.
    */
    TagRecord* getTagByName(QString const& name);

    /*
    * Gets TagRecord based on row.
    * Returns 0 if none found.
    */
    TagRecord* getTagByRow(int row);

    /*
     * Gets last TagRecord in the model.
     * Returns 0 if none found.
    */
    TagRecord* getLastTagRecord();

    /*
    * Adds a TagRecord to this model
    */
    void addTagRecord(const QString& name);

    /*
    * Returns all TagRecords held by this model
    */
    QList<DB::TagRecord*> const& getTags() const;


signals:   
   /* triggered and processed before TagRecord gets deleted */
   void aboutToBeDeleted(DB::TagRecord*);

   /* triggered and processed before TagRecords get deleted */
   void aboutToBeDeleted(const QList<DB::TagRecord*>&);

public slots:
    void deleteTag(int id);

private:
    /* validates existance of given QModelIndex for this model **/
    bool indexIsValid(const QModelIndex&) const;

    /* Clears all TagRecords from records **/
    void clear();

    Core::Api* api_;
    QSqlRelationalTableModel* source_model_;
    QList<TagRecord*> records_;
};

} // namepace Model
} // namespace DB

#endif // DB_MODEL_TAG_TABLE_MODEL_H
