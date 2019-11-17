#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QMediaPlaylist>
#include <QMap>
#include <QSet>

#include "playlist_settings.h"
#include "db/model/sound_file_table_model.h"
#include "api/extended_data_types.h"
#include "repository/sound_repository.h"

class Playlist : public QMediaPlaylist
{
    Q_OBJECT
public:    
    Playlist(QString name = "Playlist", QObject* parent = 0);
    virtual ~Playlist();

    void setSettings(const PlaylistSettings& settings);
    const PlaylistSettings& getSettings() const;

    bool addMedia(const QualifiedSoundData &sound);

    const QList<QualifiedSoundData> getSounds() const;

signals:
    void changedSettings();

private:
    QString name_;
    PlaylistSettings* settings_;
    QList<QualifiedSoundData> sounds_;
    SoundRepository* repo_;
};

#endif // PLAYLIST_H
