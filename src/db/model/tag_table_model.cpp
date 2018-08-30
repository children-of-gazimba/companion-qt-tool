#include "tag_table_model.h"

#include <QCoreApplication>
#include <QDebug>

namespace DB {
namespace Model {

TagTableModel::TagTableModel(Core::Api* api, QObject* parent)
    : QAbstractTableModel(parent)
    , api_(api)
    , source_model_(0)
    , records_()
{}

TagTableModel::~TagTableModel()
{
    clear();
}

int TagTableModel::columnCount(const QModelIndex&) const
{
    return 1; // (id/)name
}

int TagTableModel::rowCount(const QModelIndex&) const
{
    return records_.size();
}

QVariant TagTableModel::data(const QModelIndex &index, int role) const
{
    if(!indexIsValid(index))
        return QVariant();

    if(role == Qt::DisplayRole) {
        if(index.column() == 0)
            return QVariant(records_[index.row()]->name);
    }
    else if(role == Qt::EditRole) {
        if(index.column() == 0)
            return QVariant(records_[index.row()]->id);
    }

    return QVariant();
}

QVariant TagTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // horizontal header (attribute names)
    if(orientation == Qt::Horizontal) {
        if(section < 0 || section >= columnCount())
            return QVariant();

        if(role == Qt::DisplayRole) {
            if(section == 0)
                return QVariant("Name");
        }

        else if(role == Qt::EditRole) {
            if(section == 0)
                return QVariant("ID");
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

Qt::ItemFlags TagTableModel::flags(const QModelIndex &index) const
{
    if(!indexIsValid(index))
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}

bool TagTableModel::setData(const QModelIndex &, const QVariant &, int)
{
    // TODO support edit
    return false;
}

bool TagTableModel::setHeaderData(int, Qt::Orientation, const QVariant &, int)
{
    // TODO support edit
    return false;
}

bool TagTableModel::removeColumn(int, const QModelIndex&)
{
    return false;
}

bool TagTableModel::removeColumns(int, int, const QModelIndex&)
{
    return false;
}

bool TagTableModel::removeRow(int row, const QModelIndex&)
{
    if(row < 0)
        return false;

    // collect TagRecord being deleted
    DB::TagRecord* rec = getTagByRow(row);
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

bool TagTableModel::removeRows(int row, int count, const QModelIndex&)
{
    if(count < 0 || row < 0)
        return false;

    if(row < records_.size() && row+count < records_.size()) {

        // collect all TagRecords being deleted
        QList<DB::TagRecord*> recs;
        for(int i = row; i <= row+count; ++i) {
            DB::TagRecord* rec = getTagByRow(i);
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
            DB::TagRecord* rec = recs.first();
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

void TagTableModel::update()
{
    select();
}

void TagTableModel::select()
{
    if(api_ == 0) {
        qDebug() << "FAILURE: cannot select TagTableModel";
        qDebug() << " > (DB::Core::Api*) api is null";
        return;
    }

    if(records_.size() > 0)
        clear();

    source_model_ = api_->getTagTable();

    int c_id = source_model_->fieldIndex("id");
    int c_name = source_model_->fieldIndex("name");
    for(int row = 0; row < source_model_->rowCount(); ++row) {
        TagRecord* rec = new TagRecord;

        // set id
        QModelIndex index = source_model_->index(row, c_id);
        rec->id = source_model_->data(index).toInt();

        // set name
        index = source_model_->index(row, c_name);
        rec->name = source_model_->data(index).toString();

        // add to list of records
        records_.append(rec);
    }

    emit layoutAboutToBeChanged();
    emit layoutChanged();
    emit dataChanged(index(0,0), index(rowCount(), columnCount()));
}

int TagTableModel::getRowByTag(TagRecord *rec)
{
    for(int row = 0; row < records_.size(); ++row) {
        if(records_[row] == rec)
            return row;
    }
    return -1;
}

TagRecord *TagTableModel::getTagById(int id)
{
    foreach(TagRecord* rec, records_) {
        if(rec->id == id)
            return rec;
    }

    return nullptr;
}

TagRecord *TagTableModel::getTagByName(const QString &name)
{
    foreach(TagRecord* rec, records_) {
        if(rec->name.compare(name) == 0)
            return rec;
    }

    return nullptr;
}

TagRecord *TagTableModel::getTagByRow(int row)
{
    if(row < rowCount())
        return records_[row];

    return nullptr;
}

TagRecord *TagTableModel::getLastTagRecord()
{
    if(rowCount() > 0)
        return getTagByRow(rowCount() - 1);

    return 0;
}

void TagTableModel::addTagRecord(const QString& name)
{
    if(getTagByName(name) != nullptr)
        return;

    api_->insertTag(name);

    int id = api_->getTagId(name);
    if(id == -1) {
        qDebug() << "FAILURE: Unknown error adding TagRecord";
        qDebug() << " > name:" << name;
        return;
    }

    records_.append(new TagRecord(id, name));

    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

const QList<TagRecord *> &TagTableModel::getTags() const
{
    return records_;
}

void TagTableModel::deleteTag(int id)
{
    TagRecord* rec = getTagById(id);
    if(rec == 0)
        return;

    int row = getRowByTag(rec);

    if(row != -1)
        removeRow(row);
}

bool TagTableModel::indexIsValid(const QModelIndex & index) const
{
    return index.isValid() && index.row() < rowCount() && index.column() < columnCount();
}

void TagTableModel::clear()
{
    while(records_.size() > 0) {
        TagRecord* rec = records_.front();
        records_.pop_front();
        delete rec;
    }
}

} // namespace Model
} // namespace DB

