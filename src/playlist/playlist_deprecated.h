#ifndef PLAYLIST_DEPRECATED_H
#define PLAYLIST_DEPRECATED_H

#include <QMediaPlaylist>
#include <QMap>
#include <QSet>

#include "playlist_settings.h"
#include "db/model/sound_file_table_model.h"

class PlaylistDeprecated : public QMediaPlaylist
{
    Q_OBJECT
public:
    PlaylistDeprecated(QString name = "Playlist", QObject* parent = 0);
    virtual ~PlaylistDeprecated();

    void setSettings(const PlaylistSettings& settings);
    const PlaylistSettings& getSettings() const;

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
    PlaylistSettings* settings_;
    SoundFileTableModel* model_;
    QMap<QMediaContent*, SoundFileRecord*> records_;

};

#endif // PLAYLIST_DEPRECATED_H
