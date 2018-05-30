#include "spotify_handler.h"

SpotifyHandler* SpotifyHandler::instance_ = nullptr;

SpotifyHandler::SpotifyHandler()
    : remote()
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

void SpotifyHandler::play(const SpotifyRemoteController::Settings &settings)
{
    remote.setRepeat(settings.repeat_mode);
    remote.setShuffle(settings.shuffle_enabled);
    remote.playUserPlaylist(settings.playlist_uri);
}

void SpotifyHandler::stop()
{
    remote.pause();
}
