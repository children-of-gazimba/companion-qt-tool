#ifndef API_CONFIG_H
#define API_CONFIG_H

#include <QString>
#include <QUrl>

struct ApiConfig {
    QUrl server_url;
    QString access_token;
};

#endif // API_CONFIG_H
