#include "spotify_remote_controller.h"

#include "QDesktopServices"
#include <QJsonObject>
#include <QJsonDocument>
#include <QStringList>
#include <QJsonArray>

SpotifyRemoteController::SpotifyRemoteController(QObject *parent) : QObject(parent)
{
    request_handler_ = new OAuth2RequestHandler(this);
    authenticator_ = new SpotifyAuthenticator(this);

    // authenticator connections
    connect(authenticator_, &SpotifyAuthenticator::accessGranted,
            this, &SpotifyRemoteController::onAccessGranted);

    connect(authenticator_, &SpotifyAuthenticator::tokenChanged,
            this, &SpotifyRemoteController::onTokenChanged);

    connect(authenticator_, &SpotifyAuthenticator::authorizeWithBrowser,
            this, &SpotifyRemoteController::authorizeWithBrowser);

    // request handler connections
//    connect(request_handler_, &OAuth2RequestHandler::requestFinished,
//            this, &SpotifyRemoteController::onResponse);

    //    connect(request_handler_, &OAuth2RequestHandler::requestFinished,
    //            this, &SpotifyRemoteHandler::serverResponse);
}


void SpotifyRemoteController::grantAccess() {
    authenticator_->grant();
}



void SpotifyRemoteController::onAccessGranted() {
    qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
    qDebug() << "  >" << "Access granted!";
    qDebug() << "  >" << "Setting token:";
    qDebug() << "  >" << authenticator_->getToken();

    request_handler_->setRequestHeaders(authenticator_->getToken());
    emit accessGranted();
}

void SpotifyRemoteController::onTokenChanged(const QString &token) {
    request_handler_->setRequestHeaders(token);
}

void SpotifyRemoteController::onResponse(QNetworkReply *reply) {
    Q_UNUSED(reply)
//    SpotifyResponse response;

//    response.type = request_queue_.first();

//    response.message = QJsonDocument::fromJson(reply->readAll());

//    emit serverResponse(response);
}



void SpotifyRemoteController::play() {
    QString url = "me/player/play";
    request_handler_->put(url);
}

void SpotifyRemoteController::playUserPlaylist(const QString &user, const QString &playlist_id) {
    const QString url = "me/player/play";

    QJsonObject parameter_obj;
    parameter_obj["context_uri"] = QString("spotify:user:%1:playlist:%2").arg(user).arg(playlist_id);
    QJsonDocument parameter_doc(parameter_obj);

    request_handler_->put(url, parameter_doc.toJson());
}

void SpotifyRemoteController::playUserPlaylist(const QString &spotify_uri) {
    const QString url = "me/player/play";

    QJsonObject parameter_obj;
    parameter_obj["context_uri"] = spotify_uri;
    QJsonDocument parameter_doc(parameter_obj);

    request_handler_->put(url, parameter_doc.toJson());
}

void SpotifyRemoteController::playTrack(const QString &spotify_uri) {

    const QString url = "me/player/play";

    QJsonObject parameter_obj;
    QJsonArray tracks;
    tracks.append(spotify_uri);
    parameter_obj["uris"] = tracks;
    QJsonDocument parameter_doc(parameter_obj);

    request_handler_->put(url, parameter_doc.toJson());
}

void SpotifyRemoteController::pause() {
    const QString url = "me/player/pause";
    request_handler_->put(url);
}

void SpotifyRemoteController::next() {
    const QString url = "me/player/next";
    request_handler_->post(url);
}

void SpotifyRemoteController::prev() {
    const QString url = "me/player/previous";
    request_handler_->post(url);
}

void SpotifyRemoteController::setRepeat(RepeatMode mode) {

    QString url = "me/player/repeat";

    switch (mode) {
    case Track:
        url += "?state=track";
        break;
    case Context:
        url += "?state=context";
        break;
    case Off:
        url += "?state=off";
        break;
    default:
        break;
    }

    request_handler_->put(url);
}

void SpotifyRemoteController::setShuffle(bool enabled) {
    QString url = QString("me/player/shuffle?state=%1").arg(enabled);
    request_handler_->put(url);
}

void SpotifyRemoteController::setVolume(int value) {
    QString url = QString("me/player/volume?volume_percent=%1").arg(value);
    request_handler_->put(url);
}

QNetworkReply *SpotifyRemoteController::getPlaylistInfo(const QString &spotify_uri) {
    auto components = spotify_uri.split(":");

    QString url = QString("users/%1/playlists/%2").arg(components[2]).arg(components[4]);
    return request_handler_->get(url);
}

QNetworkReply *SpotifyRemoteController::getPlaylistInfo(const QString &user, const QString &playlist_id) {
    QString url = QString("users/%1/playlists/%2").arg(user).arg(playlist_id);
    return request_handler_->get(url);
}

QNetworkReply *SpotifyRemoteController::getPlaylistTracks(const QString &spotify_uri) {
    auto components = spotify_uri.split(":");
    QString url = QString("users/%1/playlists/%2/tracks").arg(components[2]).arg(components[4]);
    return request_handler_->get(url);
}

QNetworkReply *SpotifyRemoteController::getPlaylistTracks(const QString &user, const QString &playlist_id) {
    QString url = QString("users/%1/playlists/%2/tracks").arg(user).arg(playlist_id);
    return request_handler_->get(url);
}

QNetworkReply *SpotifyRemoteController::getTrackInfo(const QString &track_id) {
    QString url = QString("tracks/%1").arg(track_id);
    return request_handler_->get(url);
}
