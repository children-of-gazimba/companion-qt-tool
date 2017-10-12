#ifndef WEB_APP_CHAT_MESSAGE_MODEL_H
#define WEB_APP_CHAT_MESSAGE_MODEL_H

#include <QAbstractTableModel>

#include <QList>
#include <QMap>
#include <QColor>

#include "chat_message.h"

namespace Web {
namespace App {

class ChatMessageModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ChatMessageModel(QObject *parent = 0);
    ~ChatMessageModel();

    // BC OVERRIDES
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
    QVariant headerData(int section,
                        Qt::Orientation orientation = Qt::Horizontal,
                        int role = Qt::DisplayRole) const;

    const QColor getSenderColor(const ChatMessage& msg) const;
signals:
    void messageAdded(const Web::App::ChatMessage message);

public slots:
    void addMessage(const Web::App::ChatMessage message);

private:
    bool indexIsValid(const QModelIndex& idx) const;
    const QColor getRandomColor();

    QList<ChatMessage*> messages_;
    QMap<QString, QColor> sender_colors_;
    QList<QColor> random_color_list_;
};

} // namespace App
} // namespace Web

#endif // WEB_APP_CHAT_MESSAGE_MODEL_H
