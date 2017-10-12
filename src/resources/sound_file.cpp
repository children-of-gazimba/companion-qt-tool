#include "sound_file.h"

namespace Resources {

SoundFile::SoundFile(QFileInfo const& info, DB::ResourceDirRecord const& resource_dir)
    : ResourceFile(info, resource_dir)
{}

SoundFile::~SoundFile()
{}

DB::TableIndex SoundFile::getTableIndex()
{
    return DB::SOUND_FILE;
}

} // namespace Resources
