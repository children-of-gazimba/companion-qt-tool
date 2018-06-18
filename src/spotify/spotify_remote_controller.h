#ifndef SPOTIFY_REMOTE_HANDLER_H
#define SPOTIFY_REMOTE_HANDLER_H

#include <QObject>
#include <QQueue>

#include <QJsonDocument>

#include "oauth2_request_handler.h"

class SpotifyRemoteController : public QObject
{
        Q_OBJECT
    public:
        enum RepeatMode {Track, Context, Off};

        struct Settings
        {

                enum Category {Playlist, Track};

                Category mode;

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

        QNetworkReply *getPlaylistInfo(const QString &user, const QString &playlist_id);
        QNetworkReply *getPlaylistInfo(const QString &spotify_uri);

        QNetworkReply *getPlaylistTracks(const QString &user, const QString &playlist_id);
        QNetworkReply *getPlaylistTracks(const QString &spotify_uri);

        QNetworkReply *getTrackInfo(const QString &track_id);

    signals:
        void accessGranted();
        void authorizeWithBrowser(const QUrl &url);
//        void serverResponse(SpotifyResponse);

    public slots:
        void onAccessGranted();
        void onTokenChanged(const QString &token);
        void onResponse(QNetworkReply *reply);

    private:
        OAuth2RequestHandler *request_handler_;
        SpotifyAuthenticator *authenticator_;
};


#endif // SPOTIFY_REMOTE_HANDLER_H
