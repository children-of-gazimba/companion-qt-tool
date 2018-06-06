#ifndef SPOTIFY_HANDLER_H
#define SPOTIFY_HANDLER_H

#include <QList>

#include "spotify_remote_controller.h"
#include "tile/spotify_tile.h"

#include <QNetworkReply>

// singleton implementation taken from
// http://www.yolinux.com/TUTORIALS/C++Singleton.html

class SpotifyHandler
{
    public:
        static SpotifyHandler* instance();
        ~SpotifyHandler();

        // delete copy and move c'tors
        SpotifyHandler(const SpotifyHandler &) = delete;
        SpotifyHandler(SpotifyHandler &&) = delete;

        // delete assign operator
        void operator=(const SpotifyHandler&) = delete;
        void operator=(SpotifyHandler&&) = delete;

        void play(Tile::SpotifyTile *tile);
        void stop();

        void setVolume(int volume);

        void addTile(Tile::SpotifyTile *tile);
        void removeTile(Tile::SpotifyTile *tile);

        QNetworkReply *playlistInfo(const QString &uri);

        SpotifyRemoteController remote;

    private:

        explicit SpotifyHandler();

        Tile::SpotifyTile *active_tile_;
        QList<Tile::SpotifyTile*> existing_tiles_;
        static SpotifyHandler* instance_;
};

#endif // SPOTIFY_HANDLER_H
