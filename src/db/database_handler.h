#ifndef DB_DATABASE_HANDLER_H
#define DB_DATABASE_HANDLER_H

#include <QObject>

#include <QSqlRelationalTableModel>

#include "core/api.h"
#include "resources/sound_file.h"
#include "model/category_tree_model.h"
#include "model/sound_file_table_model.h"
#include "model/resource_dir_table_model.h"
#include "model/image_dir_table_model.h"
#include "model/preset_table_model.h"

/*
 * Class that Provides high-level interface to DB::Core::Api,
 * which is specific to this application.
 */
class DatabaseHandler : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseHandler(DB::Core::Api* api, QObject *parent = 0);

    DB::Core::Api* getApi() const;

    DB::Model::CategoryTreeModel* getCategoryTreeModel();
    DB::Model::SoundFileTableModel* getSoundFileTableModel();
    DB::Model::ResourceDirTableModel* getResourceDirTableModel();
    DB::Model::ImageDirTableModel* getImageDirTableModel();
    DB::Model::PresetTableModel* getPresetTableModel();

    /*
     * Gets a list of SoundFileRecords,
     * associated with Category referenced by given id.
    */
    QList<SoundFileRecord*> const getSoundFileRecordsByCategoryId(int category_id = -1);

signals:
    void progressChanged(int);

public slots:
    /*
    * deletes all contents of database
    */
    void deleteAll();

    /*
     * Add SoundFile to DB
    */
    void addSoundFile(QFileInfo const&, ResourceDirRecord const&);

    /*
     * Add Category to DB
    */
    void addCategory(QString name, CategoryRecord* parent = 0);

    /*
     * Add SoundFileCategory relation to DB
    */
    void addSoundFileCategory(int sound_file_id, int category_id);

    /*
     * Inserts new SoundFiles based on list given.
     * Will also insert new Categories in case any SoundFile
     * describes a new path through the category tree.
    */
    void insertSoundFilesAndCategories(QList<Resources::SoundFile> const&);

private:
    void addCategory(QStringList const& path);

    DB::Core::Api* api_;

    DB::Model::CategoryTreeModel* category_tree_model_;
    DB::Model::SoundFileTableModel* sound_file_table_model_;
    DB::Model::ResourceDirTableModel* resource_dir_table_model_;
    DB::Model::ImageDirTableModel* image_dir_table_model_;
    DB::Model::PresetTableModel* preset_table_model_;
};

#endif // DB_DATABASE_HANDLER_H
