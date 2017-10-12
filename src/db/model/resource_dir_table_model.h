#ifndef DB_MODEL_RESOURCE_DIR_TABLE_MODEL_H
#define DB_MODEL_RESOURCE_DIR_TABLE_MODEL_H

#include <QAbstractTableModel>
#include "db/core/api.h"
#include "db/table_records.h"

namespace DB {
namespace Model {

class ResourceDirTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ResourceDirTableModel(Core::Api* api, QObject *parent = 0);
    ~ResourceDirTableModel();

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
     * Gets the row of ResourceDirRecord.
     * Returns -1 if none found
    */
    int getRowByResourceDir(DB::ResourceDirRecord* rec);

    /*
     * Gets ResourceDirRecord based on path.
     * Returns 0 if none found.
    */
    ResourceDirRecord* getResourceDirByPath(QString const& path);

    /*
     * Gets ResourceDirRecord based on ID.
     * Returns 0 if none found.
    */
    ResourceDirRecord* getResourceDirById(int id);

    /*
    * Gets ResourceDirRecord based on row.
    * Returns 0 if none found.
    */
    ResourceDirRecord* getResourceDirByRow(int row);

    /*
     * Gets last ResourceDirRecord in the model.
     * Returns 0 if none found.
    */
    ResourceDirRecord* getLastResourceDirRecord();

    /*
    * Adds a ResourceDirRecord to this model
    */
    void addResourceDirRecord(const QFileInfo& info);

    /*
    * Returns all ResourceDirRecords held by this model
    */
    QList<DB::ResourceDirRecord*> const& getResourceDirs() const;


signals:   
   /* triggered and processed before ResourceDirRecord gets deleted */
   void aboutToBeDeleted(DB::ResourceDirRecord*);

   /* triggered and processed before ResourceDirRecords get deleted */
   void aboutToBeDeleted(const QList<DB::ResourceDirRecord*>&);

public slots:
    void deleteResourceDir(int id);

private:
    /* validates existance of given QModelIndex for this model **/
    bool indexIsValid(const QModelIndex&) const;

    /* Clears all ResourceDirRecords from records **/
    void clear();

    Core::Api* api_;
    QSqlRelationalTableModel* source_model_;
    QList<ResourceDirRecord*> records_;
};

} // namepace Model
} // namespace DB

#endif // DB_MODEL_RESOURCE_DIR_TABLE_MODEL_H
