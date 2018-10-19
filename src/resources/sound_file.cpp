#include "sound_file.h"

namespace Resources {

SoundFile::SoundFile(QFileInfo const& info, ResourceDirRecord const& resource_dir)
    : ResourceFile(info, resource_dir)
{}

SoundFile::~SoundFile()
{}

TableIndex SoundFile::getTableIndex()
{
    return SOUND_FILE;
}

} // namespace Resources
