#include "api.h"

namespace DB {
namespace Core {

Api::Api(QString const& db_path, QObject *parent)
    : QObject(parent)
    , db_wrapper_(0)
{
    initDB(db_path);
}

QSqlRelationalTableModel *Api::getSoundFileTable()
{
    return db_wrapper_->getTable(SOUND_FILE);
}

QSqlRelationalTableModel *Api::getCategoryTable()
{
    return db_wrapper_->getTable(CATEGORY);
}

QSqlRelationalTableModel *Api::getSoundFileCategoryTable()
{
    return db_wrapper_->getTable(SOUND_FILE_CATEGORY);
}

QSqlRelationalTableModel *Api::getResourceDirTable()
{
    return db_wrapper_->getTable(RESOURCE_DIRECTORY);
}

void Api::insertSoundFile(const QFileInfo &info, ResourceDirRecord const& resource_dir)
{
    QString rel_path = info.filePath();
    rel_path.remove(0, resource_dir.path.size());

    QString value_block  = "";
    value_block = "(name, path, relative_path) VALUES (";
    value_block += "'" + SqliteWrapper::escape(info.fileName()) + "','";
    value_block += SqliteWrapper::escape(info.filePath()) + "','";
    value_block += SqliteWrapper::escape(rel_path) + "')";

    db_wrapper_->insertQuery(SOUND_FILE, value_block);
}

void Api::insertCategory(const QString &name, int parent_id)
{
    QString value_block  = "";
    if(parent_id != -1) {
        value_block = "(name, parent_id) VALUES (";
        value_block += "'" + SqliteWrapper::escape(name) + "'," + QString::number(parent_id) + ")";
    }
    else {
        value_block = "(name) VALUES ('" + SqliteWrapper::escape(name) + "')";
    }
    db_wrapper_->insertQuery(CATEGORY, value_block);
}

void Api::insertSoundFileCategory(int sound_file_id, int category_id)
{
    QString value_block  = "";
    value_block = "(sound_file_id, category_id) VALUES (";
    value_block += QString::number(sound_file_id) + ",";
    value_block += QString::number(category_id) + ")";

    db_wrapper_->insertQuery(SOUND_FILE_CATEGORY, value_block);
}

void Api::insertResourceDir(const QFileInfo &info)
{
    QString value_block  = "";
    value_block = "(name, path) VALUES (";
    value_block += "'" + SqliteWrapper::escape(info.fileName()) + "','";
    value_block += SqliteWrapper::escape(info.filePath()) + "')";

    db_wrapper_->insertQuery(RESOURCE_DIRECTORY, value_block);
}

int Api::getSoundFileId(const QString &path)
{
    QString SELECT = "id";
    QString WHERE = "path = '" + SqliteWrapper::escape(path) + "'";
    QList<QSqlRecord> res = db_wrapper_->selectQuery(SELECT, SOUND_FILE, WHERE);
    if(res.size() > 0)
        return res[0].value(0).toInt();
    return -1;
}

int Api::getResourceDirId(const QString &path)
{
    QString SELECT = "id";
    QString WHERE = "path = '" + SqliteWrapper::escape(path) + "'";
    QList<QSqlRecord> res = db_wrapper_->selectQuery(SELECT, RESOURCE_DIRECTORY, WHERE);
    if(res.size() > 0)
        return res[0].value(0).toInt();
    return -1;
}

bool Api::soundFileExists(const QString &path, const QString &name)
{
    QString where = "path = '" + SqliteWrapper::escape(path) + "' and ";
    where += "name = '" + SqliteWrapper::escape(name) + "'";

    return db_wrapper_->selectQuery("Count(*)", SOUND_FILE, where)[0].value(0).toInt() > 0;
}

bool Api::soundFileCategoryExists(int sound_file_id, int category_id)
{
    QString where = "sound_file_id = " + QString::number(sound_file_id) + " and ";
    where += "category_id = " + QString::number(category_id) + "";

    return db_wrapper_->selectQuery("Count(*)", SOUND_FILE_CATEGORY, where)[0].value(0).toInt() > 0;
}

const QList<int> Api::getRelatedIds(TableIndex get_table, TableIndex have_table, int have_id)
{
    QList<int> ids;
    if(have_table == NONE || get_table == NONE)
        return ids;

    TableIndex relation_idx = getRelationTable(get_table, have_table);
    if(relation_idx == NONE)
        return ids;

    QString SELECT = toString(get_table) + "_id";
    QString FROM = toString(relation_idx);
    QString WHERE = toString(have_table) + "_id = " +  QString::number(have_id);

    foreach(QSqlRecord rec, db_wrapper_->selectQuery(SELECT, FROM, WHERE))
        ids.append(rec.value(0).toInt());

    return ids;
}

void Api::deleteAll()
{
    // delete sound_files
    db_wrapper_->deleteQuery(SOUND_FILE, "id > 0");

    // delete categories
    db_wrapper_->deleteQuery(CATEGORY, "id > 0");

    // delete sound_file_categories
    db_wrapper_->deleteQuery(SOUND_FILE_CATEGORY, "id > 0");

    // delete resource_dirs
    db_wrapper_->deleteQuery(RESOURCE_DIRECTORY, "id > 0");
}

TableIndex Api::getRelationTable(TableIndex first, TableIndex second)
{
    if(first == second)
        return NONE;

    if(first == SOUND_FILE || first == CATEGORY) {
        if(second == SOUND_FILE || second == CATEGORY)
            return SOUND_FILE_CATEGORY;
    }

    return NONE;
}

void Api::initDB(const QString& db_path)
{
    db_wrapper_ = new SqliteWrapper(db_path, this);
}

} // namespace Core
} // namespace DB
