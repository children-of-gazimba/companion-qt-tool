
#include "oauth2_request_handler.h"

#include <QNetworkReply>

OAuth2RequestHandler::OAuth2RequestHandler(QObject *parent)
    : QObject(parent)
    , access_granted_(false)
{
    network_manager_ = new  QNetworkAccessManager();
    connect(network_manager_, &QNetworkAccessManager::finished,
            this, &OAuth2RequestHandler::requestFinished);
}

void OAuth2RequestHandler::get(const QString &url) {
    QUrl u(base_url + url);
    request_.setUrl(u);
    QNetworkReply *reply  = network_manager_->get(request_);
    Q_UNUSED(reply);
}

void OAuth2RequestHandler::put(const QString &url, const QByteArray &parameter) {
    QUrl u(base_url + url);
    request_.setUrl(u);
    QNetworkReply *reply = network_manager_->put(request_, parameter);
    Q_UNUSED(reply);
}

void OAuth2RequestHandler::put(const QString &url) {
    QUrl u(base_url + url);
    request_.setUrl(u);
    QNetworkReply *reply = network_manager_->put(request_, QByteArray());
    Q_UNUSED(reply);
}

void OAuth2RequestHandler::post(const QString &url, const QByteArray &parameter) {
    request_.setUrl(base_url + url);
    request_.setUrl(base_url + url);
    QNetworkReply *reply  = network_manager_->post(request_, parameter);
    Q_UNUSED(reply);
}

void OAuth2RequestHandler::post(const QString &url) {
    request_.setUrl(base_url + url);
    request_.setUrl(base_url + url);
    QNetworkReply *reply = network_manager_->post(request_, QByteArray());
    Q_UNUSED(reply);
}

void OAuth2RequestHandler::setRequestHeaders(const QString &token) {
    request_ = QNetworkRequest();

    QString bearer = token;
    request_.setRawHeader("Authorization", "Bearer " + bearer.toUtf8());
    request_.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    QString user_agent = QStringLiteral("QtOAuth/1.0 (+https://www.qt.io)");
    request_.setHeader(QNetworkRequest::UserAgentHeader, user_agent);
}
