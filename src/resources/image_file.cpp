#include "image_file.h"

namespace Resources {

ImageFile::ImageFile(QFileInfo const& info, ResourceDirRecord const& resource_dir)
    : ResourceFile(info, resource_dir)
{}

ImageFile::~ImageFile()
{}

TableIndex ImageFile::getTableIndex()
{
    return IMAGE_FILE;
}

} // namespace Resources
