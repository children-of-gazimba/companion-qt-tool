#include "resource_file.h"

#include <QDebug>

namespace Resources {

ResourceFile::ResourceFile(QFileInfo const& info, DB::ResourceDirRecord const& resource_dir)
    : file_info_(info)
    , category_path_()
    , resource_dir_(resource_dir)
{
    computeCategoryPath();
}

ResourceFile::~ResourceFile()
{}

const QStringList &ResourceFile::getCategoryPath() const
{
    return category_path_;
}

const QFileInfo &ResourceFile::getFileInfo() const
{
    return file_info_;
}

const DB::ResourceDirRecord &ResourceFile::getResourceDir() const
{
    return resource_dir_;
}

void ResourceFile::computeCategoryPath()
{
    QString rel_to_base = file_info_.filePath();
    rel_to_base.replace(resource_dir_.path, "");

    category_path_ = rel_to_base.split("/", QString::SkipEmptyParts);
    category_path_.pop_back();
}

} // namespace Resources

