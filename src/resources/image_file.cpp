#include "image_file.h"

namespace Resources {

ImageFile::ImageFile(QFileInfo const& info, DB::ResourceDirRecord const& resource_dir)
    : ResourceFile(info, resource_dir)
{}

ImageFile::~ImageFile()
{}

DB::TableIndex ImageFile::getTableIndex()
{
    return DB::IMAGE_FILE;
}

} // namespace Resources
