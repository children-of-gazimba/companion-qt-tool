#include "spotify_handler.h"

SpotifyHandler* SpotifyHandler::instance_ = nullptr;

SpotifyHandler::SpotifyHandler()
    : remote()
    , active_tile_(0)
    , existing_tiles_()
{}

SpotifyHandler* SpotifyHandler::instance() {
    if(!instance_) {
        instance_ = new SpotifyHandler;
    }
    return instance_;
}

SpotifyHandler::~SpotifyHandler() {
    delete instance_;
}

void SpotifyHandler::play(Tile::SpotifyTile *selected_tile)
{

    SpotifyRemoteController::Settings current_settings = selected_tile->getSettings();
    if(active_tile_ == selected_tile) {
        remote.play();
    } else {
        switch (current_settings.mode) {
            case SpotifyRemoteController::Settings::Playlist:
                remote.playUserPlaylist(current_settings.playlist_uri);
                break;
            case SpotifyRemoteController::Settings::Track:
                remote.playTrack(current_settings.track_uri);
                break;
            default:
                qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
                qDebug() << "  >" << "Active Tile exists!";
                qDebug() << "    >" << "Error in getting settings mode!";
                break;
        }

    }

    active_tile_ = selected_tile;

    remote.setShuffle(current_settings.shuffle_enabled);
    remote.setRepeat(current_settings.repeat_mode);

    // disable all other tiles
    for(auto tile : existing_tiles_) {
        if(tile != selected_tile) {
            tile->setPlaying(false);
        }
    }

}


void SpotifyHandler::stop()
{
    remote.pause();
    for(auto tile: existing_tiles_) {
        tile->setPlaying(false);
    }
}

void SpotifyHandler::setVolume(int volume)
{
    remote.setVolume(volume);
}

void SpotifyHandler::addTile(Tile::SpotifyTile *tile)
{
    existing_tiles_.append(tile);

    qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
    qDebug() << "  >" << "added tile to active tile list";
    qDebug() << "    >" << "number of existing spotify tiles:" << existing_tiles_.length();
}

void SpotifyHandler::removeTile(Tile::SpotifyTile *tile)
{
    existing_tiles_.removeAll(tile);
}

QNetworkReply *SpotifyHandler::playlistInfo(const QString &uri)
{
    return remote.getPlaylistInfo(uri);


}

QNetworkReply *SpotifyHandler::trackInfo(const QString &track_id)
{
    return remote.getTrackInfo(track_id);
}
