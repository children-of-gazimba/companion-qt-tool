#include "resource_dir_table_model.h"

#include <QCoreApplication>
#include <QDebug>

namespace DB {
namespace Model {

ResourceDirTableModel::ResourceDirTableModel(Core::Api* api, QObject* parent)
    : QAbstractTableModel(parent)
    , api_(api)
    , source_model_(0)
    , records_()
{}

ResourceDirTableModel::~ResourceDirTableModel()
{
    clear();
}

int ResourceDirTableModel::columnCount(const QModelIndex&) const
{
    return 2; // (id/)name, path
}

int ResourceDirTableModel::rowCount(const QModelIndex&) const
{
    return records_.size();
}

QVariant ResourceDirTableModel::data(const QModelIndex &index, int role) const
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

QVariant ResourceDirTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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

Qt::ItemFlags ResourceDirTableModel::flags(const QModelIndex &index) const
{
    if(!indexIsValid(index))
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}

bool ResourceDirTableModel::setData(const QModelIndex &, const QVariant &, int)
{
    // TODO support edit
    return false;
}

bool ResourceDirTableModel::setHeaderData(int, Qt::Orientation, const QVariant &, int)
{
    // TODO support edit
    return false;
}

bool ResourceDirTableModel::removeColumn(int, const QModelIndex&)
{
    return false;
}

bool ResourceDirTableModel::removeColumns(int, int, const QModelIndex&)
{
    return false;
}

bool ResourceDirTableModel::removeRow(int row, const QModelIndex&)
{
    if(row < 0)
        return false;

    // collect ResourceDirRecord being deleted
    DB::ResourceDirRecord* rec = getResourceDirByRow(row);
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

bool ResourceDirTableModel::removeRows(int row, int count, const QModelIndex&)
{
    if(count < 0 || row < 0)
        return false;

    if(row < records_.size() && row+count < records_.size()) {

        // collect all ResourceDirRecords being deleted
        QList<DB::ResourceDirRecord*> recs;
        for(int i = row; i <= row+count; ++i) {
            DB::ResourceDirRecord* rec = getResourceDirByRow(i);
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
            DB::ResourceDirRecord* rec = recs.first();
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

void ResourceDirTableModel::update()
{
    select();
}

void ResourceDirTableModel::select()
{
    if(api_ == 0) {
        qDebug() << "FAILURE: cannot select ResourceDirTableModel";
        qDebug() << " > (DB::Core::Api*) api is null";
        return;
    }

    if(records_.size() > 0)
        clear();

    source_model_ = api_->getResourceDirTable();

    int c_id = source_model_->fieldIndex("id");
    int c_name = source_model_->fieldIndex("name");
    int c_path = source_model_->fieldIndex("path");
    for(int row = 0; row < source_model_->rowCount(); ++row) {
        ResourceDirRecord* rec = new ResourceDirRecord;

        // set id
        QModelIndex index = source_model_->index(row, c_id);
        rec->id = source_model_->data(index).toInt();

        // set name
        index = source_model_->index(row, c_name);
        rec->name = source_model_->data(index).toString();

        // set path
        index = source_model_->index(row, c_path);
        rec->path = source_model_->data(index).toString();

        // add to list of records
        records_.append(rec);
    }

    emit layoutAboutToBeChanged();
    emit layoutChanged();
    emit dataChanged(index(0,0), index(rowCount(), columnCount()));
}

int ResourceDirTableModel::getRowByResourceDir(ResourceDirRecord *rec)
{
    for(int row = 0; row < records_.size(); ++row) {
        if(records_[row] == rec)
            return row;
    }
    return -1;
}

ResourceDirRecord *ResourceDirTableModel::getResourceDirByPath(const QString &path)
{
    foreach(ResourceDirRecord* rec, records_) {
        if(rec->path.compare(path) == 0)
            return rec;
    }

    return 0;
}

ResourceDirRecord *ResourceDirTableModel::getResourceDirById(int id)
{
    foreach(ResourceDirRecord* rec, records_) {
        if(rec->id == id)
            return rec;
    }

    return 0;
}

ResourceDirRecord *ResourceDirTableModel::getResourceDirByRow(int row)
{
    if(row < rowCount())
        return records_[row];

    return 0;
}

ResourceDirRecord *ResourceDirTableModel::getLastResourceDirRecord()
{
    if(rowCount() > 0)
        return getResourceDirByRow(rowCount() - 1);

    return 0;
}

void ResourceDirTableModel::addResourceDirRecord(const QFileInfo &info)
{
    if(!info.isDir()) {
        qDebug() << "FAILURE: cannot add resourceDirRecord.";
        qDebug() << " > file info specified (" << info.filePath() << ") is not a directory.";
        return;
    }

    if(getResourceDirByPath(info.filePath()) != 0) {
        qDebug() << "FAILURE: cannot add ResourceDirRecord.";
        qDebug() << " > ResourceDir with path" << info.filePath() << "already exists.";
        return;
    }

    api_->insertResourceDir(info);

    int id = api_->getResourceDirId(info.filePath());
    if(id == -1) {
        qDebug() << "FAILURE: Unknown error adding ResourceDirRecord";
        qDebug() << " > path:" << info.filePath();
        return;
    }

    records_.append(new ResourceDirRecord(id, info.fileName(), info.filePath()));

    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

const QList<ResourceDirRecord *> &ResourceDirTableModel::getResourceDirs() const
{
    return records_;
}

void ResourceDirTableModel::deleteResourceDir(int id)
{
    ResourceDirRecord* rec = getResourceDirById(id);
    if(rec == 0)
        return;

    int row = getRowByResourceDir(rec);

    if(row != -1)
        removeRow(row);
}

bool ResourceDirTableModel::indexIsValid(const QModelIndex & index) const
{
    return index.isValid() && index.row() < rowCount() && index.column() < columnCount();
}

void ResourceDirTableModel::clear()
{
    while(records_.size() > 0) {
        ResourceDirRecord* rec = records_.front();
        records_.pop_front();
        delete rec;
    }
}

} // namespace Model
} // namespace DB

