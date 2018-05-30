#ifndef SPOTIFY_REMOTE_HANDLER_H
#define SPOTIFY_REMOTE_HANDLER_H

#include <QObject>
#include <QQueue>

#include <QJsonDocument>

#include "oauth2_request_handler.h"

struct SpotifyResponse;


class SpotifyRemoteController : public QObject
{
        Q_OBJECT
    public:
        enum SpotifyRequestType {PLAYLIST_INFO, PLAYLIST_TRACKS, TRACK_INFO, ERROR, OTHER, PLAY, PAUSE, NEXT, PREVIOUS, SHUFFLE, SET_VOLUME, PLAY_TRACK, PLAY_PLAYLIST, REPEAT_TRACK, REPEAT_CONTEXT, REPEAT_OFF};
        enum RepeatMode {Track, Context, Off};

        struct Settings
        {
                QString track_uri;
                QString playlist_uri;

                RepeatMode repeat_mode;
                bool shuffle_enabled;
        };

        explicit SpotifyRemoteController(QObject *parent = nullptr);

        void grantAccess();

        void play();
        void playUserPlaylist(const QString &user, const QString &playlist_id);
        void playUserPlaylist(const QString &spotify_uri);
        void playTrack(const QString &spotify_uri);

        void pause();
        void next();
        void prev();
        void setRepeat(RepeatMode mode);
        void setShuffle(bool enable);
        void setVolume(int value);

        void getPlaylistInfo(const QString &user, const QString &playlist_id);
        void getPlaylistInfo(const QString &spotify_uri);

        void getPlaylistTracks(const QString &user, const QString &playlist_id);
        void getPlaylistTracks(const QString &spotify_uri);

        void getTrackInfo(const QString &track_id);

    signals:
        void accessGranted();
        void authorizeWithBrowser(const QUrl &url);
        void serverResponse(SpotifyResponse);

    public slots:
        void onAccessGranted();
        void onTokenChanged(const QString &token);
        void onResponse(QNetworkReply *reply);

    private:
        QQueue<SpotifyRequestType> request_queue_;
        OAuth2RequestHandler *request_handler_;
        SpotifyAuthenticator *authenticator_;
};

struct SpotifyResponse
{
        SpotifyRemoteController::SpotifyRequestType type;
        QJsonDocument message;
};

#endif // SPOTIFY_REMOTE_HANDLER_H
