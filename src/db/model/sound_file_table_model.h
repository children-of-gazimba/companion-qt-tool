#ifndef DB_MODEL_SOUND_FILE_TABLE_MODEL_H
#define DB_MODEL_SOUND_FILE_TABLE_MODEL_H

#include <QAbstractTableModel>
#include "db/core/api.h"
#include "db/table_records.h"

namespace DB {
namespace Model {

/*
 * Class derived from QAbstractTableModel.
 * Builds a tablemodel based on the sound_file db table of this application.
 * Provides convenience functions for accessing & managing SoundFileRecords maintained by it.
*/

class SoundFileTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    SoundFileTableModel(Core::Api* api, QObject* parent = 0);
    ~SoundFileTableModel();

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
     * Gets the row of SoundFileRecord.
     * Returns -1 if none found
    */
    int getRowBySoundFile(DB::SoundFileRecord* rec);

    /*
     * Gets SoundFileRecord based on path.
     * Returns 0 if none found.
    */
    SoundFileRecord* getSoundFileByPath(QString const& path);

    /*
     * Gets SoundFileRecord based on ID.
     * Returns 0 if none found.
    */
    SoundFileRecord* getSoundFileById(int id);

    /*
     * Gets SoundFileRecord based on row.
     * Returns 0 if none found.
    */
    SoundFileRecord* getSoundFileByRow(int row);

    /*
     * gets all SoundFileRecords with relative path
     * equal to rel_path.
    */
    QList<SoundFileRecord*> const getSoundFilesByRelativePath(QString const& rel_path);

    /*
     * Gets last SoundFileRecord in the model.
     * Returns 0 if none found.
    */
    SoundFileRecord* getLastSoundFileRecord();

    /*
    * Adds a SoundFileRecord to this model
    */
    void addSoundFileRecord(
        const QFileInfo& info,
        const ResourceDirRecord& resource_dir
    );

    /*
    * Returns all SoundFileRecords held by this model
    */
    QList<DB::SoundFileRecord*> const& getSoundFiles() const;

public slots:
    void deleteSoundFile(int id);

signals:
    /* triggered and processed before SoundFileRecord gets deleted */
    void aboutToBeDeleted(DB::SoundFileRecord*);

    /* triggered and processed before SoundFileRecords get deleted */
    void aboutToBeDeleted(const QList<DB::SoundFileRecord*>&);

private:
    /* validates existance of given QModelIndex for this model **/
    bool indexIsValid(const QModelIndex&) const;

    /* Clears all SoundFileRecords from records **/
    void clear();

    Core::Api* api_;
    QSqlRelationalTableModel* source_model_;
    QList<SoundFileRecord*> records_;
};

} // namespace Model
} // namespace DB

#endif // DB_MODEL_SOUND_FILE_TABLE_MODEL_H
