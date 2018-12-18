#ifndef PLAYLIST_PLAYLIST_PLAYER_H
#define PLAYLIST_PLAYLIST_PLAYER_H

#include <QMediaPlayer>
#include <QTimer>

#include "playlist/playlist.h"
#include "playlist/playlist_settings.h"

class PlaylistPlayer : public QMediaPlayer
{
    Q_OBJECT
public:
    PlaylistPlayer(QObject* parent = 0);
    //explicit CustomMediaPlayer(QObject* parent = 0, Flags* flags = 0);

    Playlist *getPlaylist() const;
    void setPlaylist(Playlist* playlist);

signals:
    void playerActivationToggled(bool state);

public slots:
    void play();
    void onCurrentMediaIndexChanged(int position);
    void onMediaSettingsChanged();
    void onMediaVolumeChanged(int val);
    void onDelayIsOver();

    void activate();
    void deactivate();
    void setActivation(bool flag);

private:
    int getRandomIntInRange(int min, int max);
    static int getMasteredVolume(int v, int master_v);
    static int getMasteredVolume(const PlaylistSettings& settings);

    bool activated_;
    int current_content_index_;
    bool delay_flag_;
    int delay_;
    QTimer* delay_timer_;
};

#endif // PLAYLIST_PLAYLIST_PLAYER_H
