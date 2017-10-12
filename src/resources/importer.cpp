#include "importer.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QDebug>
#include <QDirIterator>
#include <QDir>

namespace Resources {

Importer::Importer(DB::Model::ResourceDirTableModel* model, QObject *parent)
    : QObject(parent)
    , model_(model)
{
}

void Importer::parseFolder(const QUrl &url, const DB::ResourceDirRecord& resource_dir)
{
    QList<Resources::SoundFile> files;
    if(url.isValid() && url.isLocalFile())
    {
        QDir d(url.toLocalFile());
        QDirIterator it(d.absolutePath(), QStringList() << "*.mp3" << "*.wma" << "*.wav", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext())
            files.append(Resources::SoundFile(QFileInfo(it.next()), resource_dir));
    }
    emit folderImported(files);
    emit folderImported();
}

void Importer::startBrowseFolder(bool)
{
    QUrl url = QFileDialog::getExistingDirectoryUrl(0, tr("Open Resource Directory"));
    if(url.isValid()) {
        DB::ResourceDirRecord* rec = createOrGetResourceDir(url);
        if(rec != 0)
            parseFolder(url, *rec);
    }
}

DB::ResourceDirRecord* Importer::createOrGetResourceDir(const QUrl &url)
{
    if(url.isValid() && url.isLocalFile()) {
        QDir d(url.toLocalFile());
        d.cdUp();
        DB::ResourceDirRecord* rec = model_->getResourceDirByPath(d.absolutePath());
        if(rec == 0) {
            model_->addResourceDirRecord(QFileInfo(d.absolutePath()));
            rec = model_->getResourceDirByPath(d.absolutePath());
        }
        return rec;
    }

    return 0;
}

} // namespace Resources
