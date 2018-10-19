#ifndef DB_DATABASE_HANDLER_H
#define DB_DATABASE_HANDLER_H

#include <QObject>

#include <QSqlRelationalTableModel>

#include "core/database_api.h"
#include "resources/sound_file.h"
#include "model/category_tree_model.h"
#include "model/sound_file_table_model.h"
#include "model/resource_dir_table_model.h"
#include "model/image_dir_table_model.h"
#include "model/preset_table_model.h"

/*
 * Class that Provides high-level interface to Api,
 * which is specific to this application.
 */
class DatabaseHandler : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseHandler(DatabaseApi* api, QObject *parent = 0);

    DatabaseApi* getApi() const;

    CategoryTreeModel* getCategoryTreeModel();
    SoundFileTableModel* getSoundFileTableModel();
    ResourceDirTableModel* getResourceDirTableModel();
    ImageDirTableModel* getImageDirTableModel();
    PresetTableModel* getPresetTableModel();

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

    DatabaseApi* api_;

    CategoryTreeModel* category_tree_model_;
    SoundFileTableModel* sound_file_table_model_;
    ResourceDirTableModel* resource_dir_table_model_;
    ImageDirTableModel* image_dir_table_model_;
    PresetTableModel* preset_table_model_;
};

#endif // DB_DATABASE_HANDLER_H
