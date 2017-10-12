#ifndef RESOURCES_RESOURCE_FILE_H
#define RESOURCES_RESOURCE_FILE_H

#include <QFileInfo>
#include <QStringList>

#include "db/table_records.h"

namespace Resources {

/*
 * Abstract Class encapsulating a loaded resource file.
 * Can compute a category tree path based on relative folder structure.
 * Used as base class for
 * SoundFile (see sound_file.h) and ImageFile (see image_file.h).
*/
class ResourceFile
{
public:
    ResourceFile(QFileInfo const&, DB::ResourceDirRecord const&);
    virtual ~ResourceFile();

    /*
     * Gets the category tree path of this instance.
     * Has to be computed given a root ressource path.
     * (See computeCategorypath(QString const&))
    */
    QStringList const& getCategoryPath() const;

    /* Gets the file info of this instance. */
    QFileInfo const& getFileInfo() const;

    /* Gets the resource directory of this instace. */
    DB::ResourceDirRecord const& getResourceDir() const;

    /* Pure virtual function to retrieve table index of this instance. */
    virtual DB::TableIndex getTableIndex() = 0;

protected:
    /*
     * Determines the category tree path based on
     * relative folder structure the file resides in.
     * Given the resource folder as a root directory.
     * Root folder will not be used as a Category.
    */
    void computeCategoryPath();

    QFileInfo file_info_;
    QStringList category_path_;
    DB::ResourceDirRecord resource_dir_;
};

} // namespace DB

#endif // RESOURCES_RESOURCE_FILE_H
