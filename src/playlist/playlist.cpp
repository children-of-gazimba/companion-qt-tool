#include "playlist.h"

#include <QDebug>

#include "misc/json_mime_data_parser.h"

Playlist::Playlist(QString name, QObject* parent)
    : QMediaPlaylist(parent)
    , name_(name)
    , settings_(0)
    , model_(0)
    , records_()
{
    settings_ = new PlaylistSettings;
}

Playlist::~Playlist()
{
    QList<QMediaContent*> keys = records_.keys();
    while(keys.size() > 0) {
        records_.remove(keys[0]);
        delete keys[0];
        keys.pop_front();
    }
    delete settings_;
}

void Playlist::setSettings(const PlaylistSettings &settings)
{
    settings_->copyFrom(settings);
    emit changedSettings();
}

const PlaylistSettings& Playlist::getSettings() const
{
    return *settings_;
}

void Playlist::setSoundFileModel(SoundFileTableModel *m)
{
    model_ = m;

    connect(this, SIGNAL(mediaAboutToBeRemoved(int,int)),
            this, SLOT(onMediaAboutToBeRemoved(int,int)));
}

const SoundFileTableModel *Playlist::getSoundFileModel() const
{
    return model_;
}

bool Playlist::addMedia(const SoundFileRecord &rec)
{
    return addMedia(rec.id);
}

bool Playlist::addMedia(int record_id)
{
    if(model_ == 0)
        return false;
    SoundFileRecord* rec = model_->getSoundFileById(record_id);
    if(rec == 0)
        return false;

    QMediaContent* c = new QMediaContent(QUrl("file:///" + rec->path));

    // this should very rarely occur
    QList<QMediaContent*> delete_recs;
    foreach(QMediaContent* r_c, records_.keys()) {
        if(*r_c == *c) {
            QString old_str = JsonMimeDataParser::toJsonMimeData(records_[r_c])->text();
            QString new_str = JsonMimeDataParser::toJsonMimeData(rec)->text();
            qDebug() << "Sound file was already set for media content.";
            qDebug() << " > Record will be replaced.";
            qDebug() << " > old:" << old_str;
            qDebug() << " > new:" << new_str;
            delete_recs.append(r_c);
        }
    }
    while(delete_recs.size() > 0) {
        records_.remove(delete_recs[0]);
        delete delete_recs[0];
        delete_recs.pop_front();
    }
    //

    if(!QMediaPlaylist::addMedia(*c))
        return false;

    records_[c] = rec;

    return true;
}

const QList<SoundFileRecord *> Playlist::getSoundFileList(bool unique)
{
    QList<SoundFileRecord*> sf_list;

    if(unique) {
        foreach(SoundFileRecord* rec, records_.values()) {
            if(sf_list.contains(rec))
                continue;
            sf_list.append(rec);
        }
    }
    else {
        for(int i = 0; i < mediaCount(); ++i) {
            QMediaContent c = media(i);
            foreach(QMediaContent* r_c, records_.keys()) {
                if(*r_c == c) {
                    sf_list.append(records_[r_c]);
                    break;
                }
            }
        }
    }

    return sf_list;
}

void Playlist::onMediaAboutToBeRemoved(int start, int end)
{
    for(int i = start; i <= end; ++i) {
        QMediaContent c = media(i);
        QMediaContent* remove_key = 0;
        foreach(QMediaContent* r_c, records_.keys()) {
            if(*r_c == c) {
                remove_key = r_c;
            }
        }
        if(remove_key != 0) {
            records_.remove(remove_key);
            delete remove_key;
        }
    }
}
