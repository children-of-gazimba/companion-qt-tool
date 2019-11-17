#include "playlist.h"

#include <QDebug>

#include "json/json_mime_data_parser.h"
#include "resources/lib.h"

Playlist::Playlist(QString name, QObject* parent)
    : QMediaPlaylist(parent)
    , name_(name)
    , settings_(new PlaylistSettings)
    , repo_(new SoundRepository(this))
{}

Playlist::~Playlist()
{
    delete settings_;
}

void Playlist::setSettings(const PlaylistSettings &settings)
{
    settings_->copyFrom(settings);
    emit changedSettings();
}

const PlaylistSettings& Playlist::getSettings() const
{
    return *settings_;
}

bool Playlist::addMedia(const QualifiedSoundData &sound)
{
    auto cfg = (*Resources::Lib::API_CONFIG_MODEL)[sound.server];
    repo_->setApiToken(cfg.access_token);
    repo_->setServerUrl(cfg.server_url);

    QNetworkRequest req;
    QSslConfiguration conf = QSslConfiguration::defaultConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    req.setUrl(repo_->getStreamUrl(sound));
    req.setSslConfiguration(conf);
    if(!QMediaPlaylist::addMedia(req))
        return false;

    sounds_.append(sound);
    return true;
}

const QList<QualifiedSoundData> Playlist::getSounds() const
{
    return sounds_;
}
