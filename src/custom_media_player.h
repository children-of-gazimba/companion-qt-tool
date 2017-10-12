#ifndef CUSTOM_MEDIA_PLAYER_H
#define CUSTOM_MEDIA_PLAYER_H

#include <QMediaPlayer>
#include <QTimer>

#include "playlist/media_playlist.h"
#include "playlist/settings.h"

class CustomMediaPlayer : public QMediaPlayer
{
    Q_OBJECT
public:
    CustomMediaPlayer(QObject* parent = 0);
    //explicit CustomMediaPlayer(QObject* parent = 0, Flags* flags = 0);

    Playlist::MediaPlaylist *getCustomPlaylist() const;

signals:
    void toggledPlayerActivation(bool state);

public slots:
    void play();
    void setPlaylist(Playlist::MediaPlaylist* playlist);
    void currentMediaIndexChanged(int position);
    void mediaSettingsChanged();
    void mediaVolumeChanged(int val);
    void delayIsOver();

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

#endif // CUSTOM_MEDIA_PLAYER_H
