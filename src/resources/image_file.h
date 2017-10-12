#ifndef RESOURCES_IMAGE_FILE_H
#define RESOURCES_IMAGE_FILE_H

#include "resource_file.h"

namespace Resources {

/*
 * Class encapsulating a loaded image file resource.
 * Used mainly as a data transfer object.
 * See BC for more info.
*/
class ImageFile : public ResourceFile
{
public:
    ImageFile(QFileInfo const&, DB::ResourceDirRecord const&);
    ~ImageFile();

    /*virtual*/ DB::TableIndex getTableIndex();
};

} // namespace Resources

#endif // RESOURCES_IMAGE_FILE_H
