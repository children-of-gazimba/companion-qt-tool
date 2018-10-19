#ifndef COMPANION_MEDIA_PLAYER_H
#define COMPANION_MEDIA_PLAYER_H

#include <QMediaPlayer>
#include <QTimer>

#include "playlist/companion_playlist.h"
#include "playlist/companion_playlist_settings.h"

class CompanionPlaylistPlayer : public QMediaPlayer
{
    Q_OBJECT
public:
    CompanionPlaylistPlayer(QObject* parent = 0);
    //explicit CustomMediaPlayer(QObject* parent = 0, Flags* flags = 0);

    CompanionPlaylist *getPlaylist() const;
    void setPlaylist(CompanionPlaylist* playlist);

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

    bool activated_;
    int current_content_index_;
    bool delay_flag_;
    int delay_;
    QTimer* delay_timer_;
};

#endif // COMPANION_MEDIA_PLAYER_H
