#ifndef RESOURCES_SOUND_FILE_H
#define RESOURCES_SOUND_FILE_H

#include "resource_file.h"

namespace Resources {

/*
 * Class encapsulating a loaded sound file resource.
 * Used mainly as a data transfer object.
 * See BC for more info.
*/
class SoundFile : public ResourceFile
{
public:
    SoundFile(QFileInfo const&, DB::ResourceDirRecord const&);
    ~SoundFile();

    /*virtual*/ DB::TableIndex getTableIndex();
};

} // namespace Resources

#endif // RESOURCES_SOUND_FILE_H
