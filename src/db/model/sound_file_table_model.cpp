#include "sound_file_table_model.h"

#include <QCoreApplication>
#include <QDebug>

namespace DB {
namespace Model {

SoundFileTableModel::SoundFileTableModel(Core::Api* api, QObject* parent)
    : QAbstractTableModel(parent)
    , api_(api)
    , source_model_(0)
    , records_()
{}

SoundFileTableModel::~SoundFileTableModel()
{
    clear();
}

int SoundFileTableModel::columnCount(const QModelIndex&) const
{
    return 2; // (id/)name, path
}

int SoundFileTableModel::rowCount(const QModelIndex&) const
{
    return records_.size();
}

QVariant SoundFileTableModel::data(const QModelIndex &index, int role) const
{
    if(!indexIsValid(index))
        return QVariant();

    if(role == Qt::DisplayRole) {
        if(index.column() == 0)
            return QVariant(records_[index.row()]->name);
        else if(index.column() == 1)
            return QVariant(records_[index.row()]->path);
    }
    else if(role == Qt::EditRole) {
        if(index.column() == 0)
            return QVariant(records_[index.row()]->id);
        else if(index.column() == 1)
            return QVariant(records_[index.row()]->path);
    }

    return QVariant();
}

QVariant SoundFileTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // horizontal header (attribute names)
    if(orientation == Qt::Horizontal) {
        if(section < 0 || section >= columnCount())
            return QVariant();

        if(role == Qt::DisplayRole) {
            if(section == 0)
                return QVariant("Name");
            else if(section == 1)
                return QVariant("Path");
        }

        else if(role == Qt::EditRole) {
            if(section == 0)
                return QVariant("ID");
            else if(section == 1)
                return QVariant("Path");
        }
    }

    // vertical header (number of record)
    else {
        if(section < 0 || section >= rowCount())
            return QVariant();
        if(role == Qt::DisplayRole || role == Qt::EditRole)
            return QVariant(section);
    }

    return QVariant();
}

Qt::ItemFlags SoundFileTableModel::flags(const QModelIndex &index) const
{
    if(!indexIsValid(index))
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}

bool SoundFileTableModel::setData(const QModelIndex &, const QVariant &, int)
{
    // TODO support edit
    return false;
}

bool SoundFileTableModel::setHeaderData(int, Qt::Orientation, const QVariant &, int)
{
    // TODO support edit
    return false;
}

bool SoundFileTableModel::removeColumn(int, const QModelIndex&)
{
    return false;
}

bool SoundFileTableModel::removeColumns(int, int, const QModelIndex&)
{
    return false;
}

bool SoundFileTableModel::removeRow(int row, const QModelIndex&)
{
    if(row < 0)
        return false;

    // collect SOundFileRecord being deleted
    DB::SoundFileRecord* rec = getSoundFileByRow(row);
    if(rec != 0) {

        // signal deletion
        emit aboutToBeDeleted(rec);
        QCoreApplication::processEvents();

        // remove from source
        if(!source_model_->removeRow(row, QModelIndex()))
            return false;

        // remove from storage & delete pointer
        records_.removeAt(row);
        delete rec;
        rec = 0;

        // call super class function
        QAbstractTableModel::removeRow(row, QModelIndex());

        return true;
    }

    return false;
}

bool SoundFileTableModel::removeRows(int row, int count, const QModelIndex&)
{
    if(count < 0 || row < 0)
        return false;

    if(row < records_.size() && row+count < records_.size()) {

        // collect all SoundFileRecords being deleted
        QList<DB::SoundFileRecord*> recs;
        for(int i = row; i <= row+count; ++i) {
            DB::SoundFileRecord* rec = getSoundFileByRow(i);
            if(rec == 0)
                return false;
            recs.append(rec);
        }

        // signal deletion
        emit aboutToBeDeleted(recs);
        QCoreApplication::processEvents();

        // remove from source model
        if(!source_model_->removeRows(row, count, QModelIndex()))
            return false;

        // remove from storage
        for(int i = row+count; i >= row; --i)
            records_.removeAt(i);

        // delete pointers
        while(recs.size() > 0) {
            DB::SoundFileRecord* rec = recs.first();
            delete rec;
            rec = 0;
            recs.pop_front();
        }

        // call super class function
        QAbstractTableModel::removeRows(row, count, QModelIndex());

        return true;
    }

    return false;
}

void SoundFileTableModel::update()
{
    select();
}

void SoundFileTableModel::select()
{
    if(api_ == 0) {
        qDebug() << "FAILURE: cannot select SoundFileTableModel";
        qDebug() << " > (DB::Core::Api*) api is null";
        return;
    }

    if(records_.size() > 0)
        clear();

    source_model_ = api_->getSoundFileTable();

    int c_id = source_model_->fieldIndex("id");
    int c_name = source_model_->fieldIndex("name");
    int c_path = source_model_->fieldIndex("path");
    int c_rel_path = source_model_->fieldIndex("relative_path");
    for(int row = 0; row < source_model_->rowCount(); ++row) {
        SoundFileRecord* rec = new SoundFileRecord;

        // set id
        QModelIndex index = source_model_->index(row, c_id);
        rec->id = source_model_->data(index).toInt();

        // set name
        index = source_model_->index(row, c_name);
        rec->name = source_model_->data(index).toString();

        // set path
        index = source_model_->index(row, c_path);
        rec->path = source_model_->data(index).toString();

        // set relative path
        index = source_model_->index(row, c_rel_path);
        rec->relative_path = source_model_->data(index).toString();

        // add to list of records
        records_.append(rec);
    }

    emit layoutAboutToBeChanged();
    emit layoutChanged();
    emit dataChanged(index(0,0), index(rowCount(), columnCount()));
}

int SoundFileTableModel::getRowBySoundFile(SoundFileRecord *rec)
{
    for(int row = 0; row < records_.size(); ++row) {
        if(records_[row] == rec)
            return row;
    }
    return -1;
}

SoundFileRecord *SoundFileTableModel::getSoundFileByPath(const QString &path)
{
    foreach(SoundFileRecord* rec, records_) {
        if(rec->path.compare(path) == 0)
            return rec;
    }

    return 0;
}

SoundFileRecord *SoundFileTableModel::getSoundFileById(int id)
{
    foreach(SoundFileRecord* rec, records_) {
        if(rec->id == id)
            return rec;
    }

    return 0;
}

SoundFileRecord *SoundFileTableModel::getSoundFileByRow(int row)
{
    if(row < rowCount())
        return records_[row];

    return 0;
}

QList<SoundFileRecord *> const SoundFileTableModel::getSoundFilesByRelativePath(const QString &rel_path)
{
    QList<SoundFileRecord*> sound_files;
    foreach(SoundFileRecord* rec, records_) {
        if(rec->relative_path.compare(rel_path) == 0)
            sound_files.append(rec);
    }
    return sound_files;
}

SoundFileRecord *SoundFileTableModel::getLastSoundFileRecord()
{
    if(rowCount() > 0)
        return getSoundFileByRow(rowCount() - 1);

    return 0;
}

void SoundFileTableModel::addSoundFileRecord(const QFileInfo& info, const ResourceDirRecord& resource_dir)
{
    if(getSoundFileByPath(info.filePath()) != 0) {
        qDebug() << "FAILURE: cannot add SoundFileRecord.";
        qDebug() << " > SoundFile with path" << info.filePath() << "already exists.";
        return;
    }

    if(!info.filePath().startsWith(resource_dir.path)) {
        qDebug() << "FAILURE: cannot add SoundFileRecord.";
        qDebug() << " > ResourceDir" << resource_dir.path << "does not contain SoundFile" << info.filePath();
        return;
    }

    api_->insertSoundFile(info, resource_dir);

    int id = api_->getSoundFileId(info.filePath());
    if(id == -1) {
        qDebug() << "FAILURE: Unknown error adding SoundFileRecord";
        qDebug() << " > path:" << info.filePath();
        return;
    }

    QString rel_path = info.filePath();
    rel_path.remove(0, resource_dir.path.size());

    records_.append(new SoundFileRecord(id, info.fileName(), info.filePath(), rel_path));

    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

const QList<SoundFileRecord *> &SoundFileTableModel::getSoundFiles() const
{
    return records_;
}

void SoundFileTableModel::deleteSoundFile(int id)
{
    SoundFileRecord* rec = getSoundFileById(id);
    if(rec == 0)
        return;

    int row = getRowBySoundFile(rec);

    if(row != -1)
        removeRow(row);
}

bool SoundFileTableModel::indexIsValid(const QModelIndex & index) const
{
    return index.isValid() && index.row() < rowCount() && index.column() < columnCount();
}

void SoundFileTableModel::clear()
{
    while(records_.size() > 0) {
        SoundFileRecord* rec = records_.front();
        records_.pop_front();
        delete rec;
    }
}

} // namespace Model
} // namespace DB

