#include "spotify_handler.h"

SpotifyHandler* SpotifyHandler::instance_ = nullptr;

SpotifyHandler::SpotifyHandler()
    : remote()
    , current_settings()
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

    if(settings.playlist_uri != "") {

        if(settings.playlist_uri == current_settings.playlist_uri)
            remote.play();
        else
            remote.playUserPlaylist(settings.playlist_uri);

    } else if (settings.track_uri != "") {

        if(settings.track_uri == current_settings.track_uri)
            remote.play();
        else
            remote.playTrack(settings.track_uri);
    } else {
        qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
        qDebug() << "  >" << "something went wrong, there are no uri's given in the current settings object";
    }

    remote.setRepeat(settings.repeat_mode);
    remote.setShuffle(settings.shuffle_enabled);

    current_settings = settings;
}

void SpotifyHandler::stop()
{
    remote.pause();
}

void SpotifyHandler::setVolume(int volume)
{
    remote.setVolume(volume);
}
