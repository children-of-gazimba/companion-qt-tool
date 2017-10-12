#ifndef WEB_APP_CHAT_MESSAGE_H
#define WEB_APP_CHAT_MESSAGE_H

#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QList>

namespace Web {
namespace App {

/**
 * @brief The ChatMessage struct serves as a data transfer object
 * for chat messages used by the web host interface.
 */
struct ChatMessage {
    QString from;
    QString content;
    QDateTime time_stamp;

    ChatMessage(const QString& f = "", const QString& c = "", QDateTime t = QDateTime::currentDateTime())
        : from(f)
        , content(c)
        , time_stamp(t)
    {}

    ChatMessage(const ChatMessage& c)
        : from(c.from)
        , content(c.content)
        , time_stamp(c.time_stamp)
    {}

    const QVariant operator[](int x) {
        QVariant res;
        switch(x) {
            case 0:
                res = QVariant(from); break;
            case 1:
                res = QVariant(content); break;
            case 2:
                res = QVariant(time_stamp); break;
            default:
                break;
        }
        return res;
    }

    static const QList<QString> getAttributeNames() {
        QList<QString> names;
        names.append("From");
        names.append("Content");
        names.append("Time Stamp");
        return names;
    }
};

} // namespace App
} // namespace Web

#endif // WEB_APP_CHAT_MESSAGE_H

