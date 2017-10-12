#ifndef TEST_MULTI_TRACK_MEDIA_PLAYER_H
#define TEST_MULTI_TRACK_MEDIA_PLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QMap>
#include <QVBoxLayout>

#include "audio_widget.h"

namespace _TEST {

/*
 * Class that manages a number of media players
 * Exposes some functionality of each QMediaPlayer
 * by wrapping certain QMediaPlayer features and
 * making them callable with the extension of the designated
 * player ID (e.g. void play(int id)).
 * Offers UI control for each player to interact
 * with multiple players at once.
 * Automatically Manages an overall sound volume.
*/
class MultiTrackMediaPlayer : public QWidget
{
    Q_OBJECT

public:
    MultiTrackMediaPlayer(QWidget* parent = 0);
    ~MultiTrackMediaPlayer();

    /* wrapped QMediaPlayer functionality **/
    QMediaContent const media(int id) const;

public slots:
    /* adding and removing tracks (Player + Widget) **/
    void addTrack(int);
    void removeTrack(int);

    /* wrapped QMediaPlayer functionality **/
    void setMedia(int id, QMediaContent const& content,
                  QIODevice* device = 0);
    void setVolume(int id, int volume);
    void play(int id);
    void stop(int id);
    void pause(int id);

private:
    /* adding and removing players **/
    void addPlayer(int id);
    void removePlayer(int id);

    /* adding and removing widgets **/
    void addWidget(int id);
    void removeWidget(int id);

    /* remove all players and widgets **/
    void removeAll();

    /* init empty widget layout **/
    void initLayout();

    QMap<int, QMediaPlayer*> active_players_;
    QMap<int, AudioWidget*> active_widgets_;
    QVBoxLayout* widget_layout_;
};

} // namespace _TEST

#endif // TEST_MULTI_TRACK_MEDIA_PLAYER_H
