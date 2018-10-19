#ifndef PLAYLIST_COMPANION_PLAYLIST_SETTINGS_H
#define PLAYLIST_COMPANION_PLAYLIST_SETTINGS_H

#include <QString>

/*
 * Describes in which order a playlist can play its sound files.
 * ordered: simply follows the imported order.
 * shuffle: random order.
 * weigthed: component looks up sound weights and plays probablistic.
*/
enum PlayOrder {
    ORDERED,
    SHUFFLE,
    WEIGHTED
};

/*
 * Struct describing how a playlist should play its sound files.
*/
struct CompanionPlaylistSettings {

    QString name;
    PlayOrder order;
    bool loop_flag;
    bool interval_flag;
    int min_delay_interval;
    int max_delay_interval;
    int volume;
    QString image_path;

    CompanionPlaylistSettings()
        : name("Settings")
        , order(PlayOrder::ORDERED)
        , loop_flag(true)
        , interval_flag(false)
        , min_delay_interval(0)
        , max_delay_interval(0)
        , volume(100)
        , image_path()
    {}

    CompanionPlaylistSettings(QString n,PlayOrder ord, bool loop, bool interval, int min_interval, int max_interval, int vol, QString img_path)
        : name(n)
        , order(ord)
        , loop_flag(loop)
        , interval_flag(interval)
        , min_delay_interval(min_interval)
        , max_delay_interval(max_interval)
        , volume(vol)
        , image_path(img_path)
    {}

    void copyFrom(const CompanionPlaylistSettings& settings)
    {
        name = settings.name;
        order = settings.order;
        loop_flag = settings.loop_flag;
        interval_flag = settings.interval_flag;
        min_delay_interval = settings.min_delay_interval;
        max_delay_interval = settings.max_delay_interval;
        volume = settings.volume;
        image_path = settings.image_path;
    }
};

#endif // PLAYLIST_COMPANION_PLAYLIST_SETTINGS_H

