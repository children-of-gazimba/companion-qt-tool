#ifndef PLAYLIST_PLAYLIST_H
#define PLAYLIST_PLAYLIST_H

#include <QMediaPlaylist>
#include <QMap>
#include <QSet>

#include "companion_playlist_settings.h"
#include "db/model/sound_file_table_model.h"

class CompanionPlaylist : public QMediaPlaylist
{
    Q_OBJECT
public:
    CompanionPlaylist(QString name = "Playlist", QObject* parent = 0);
    virtual ~CompanionPlaylist();

    void setSettings(const CompanionPlaylistSettings& settings);
    const CompanionPlaylistSettings& getSettings() const;

    /**
     * Set database for the playlist
     */
    void setSoundFileModel(SoundFileTableModel* m);
    const SoundFileTableModel* getSoundFileModel() const;

    bool addMedia(const SoundFileRecord& rec);
    bool addMedia(int record_id);

    const QList<SoundFileRecord*> getSoundFileList(bool unique = false);

signals:
    void changedSettings();

private slots:
    void onMediaAboutToBeRemoved(int start, int end);

private:
    QString name_;
    CompanionPlaylistSettings* settings_;
    SoundFileTableModel* model_;
    QMap<QMediaContent*, SoundFileRecord*> records_;

};

#endif // PLAYLIST_PLAYLIST_H
