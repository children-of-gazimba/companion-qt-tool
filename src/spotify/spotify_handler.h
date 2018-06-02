#ifndef SPOTIFY_HANDLER_H
#define SPOTIFY_HANDLER_H

#include "spotify_remote_controller.h"

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

        void play(const SpotifyRemoteController::Settings &settings);
        void stop();


        SpotifyRemoteController remote;

    private:
        explicit SpotifyHandler();

        SpotifyRemoteController::Settings current_settings;
        static SpotifyHandler* instance_;
};

#endif // SPOTIFY_HANDLER_H
