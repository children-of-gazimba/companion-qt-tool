#include "chat_message_model.h"

namespace Web {
namespace App {

ChatMessageModel::ChatMessageModel(QObject *parent)
    : QAbstractTableModel(parent)
    , messages_()
    , sender_colors_()
    , random_color_list_()
{}

ChatMessageModel::~ChatMessageModel()
{
    ChatMessage* msg = 0;
    while(messages_.size() != 0) {
        msg = messages_.back();
        messages_.pop_back();
        delete msg;
        msg = 0;
    }
}

int ChatMessageModel::rowCount(const QModelIndex&) const
{
    return messages_.size();
}

int ChatMessageModel::columnCount(const QModelIndex&) const
{
    return ChatMessage::getAttributeNames().size();
}

QVariant ChatMessageModel::data(const QModelIndex& index, int) const
{
    if(!indexIsValid(index))
        return QVariant();

    return (*messages_[index.row()])[index.column()];
}

QVariant ChatMessageModel::headerData(int section, Qt::Orientation orientation, int) const
{
    QVariant res;
    if(orientation == Qt::Horizontal) {
        if(section < columnCount() && section >= 0)
            res = QVariant(ChatMessage::getAttributeNames()[section]);
    }
    else {
        if(section < rowCount() && section >= 0)
            res = QVariant(section);
    }
    return res;
}

const QColor ChatMessageModel::getSenderColor(const ChatMessage &msg) const
{
    if(sender_colors_.contains(msg.from))
        return sender_colors_[msg.from];
    return QColor();
}

void ChatMessageModel::addMessage(const ChatMessage message)
{
    messages_.append(new ChatMessage(message));
    if(!sender_colors_.contains(message.from))
        sender_colors_[message.from] = getRandomColor();
    emit dataChanged(index(rowCount()-2, 0), index(rowCount()-1, columnCount()));
    emit layoutAboutToBeChanged();
    emit layoutChanged();
    emit messageAdded(message);
}

bool ChatMessageModel::indexIsValid(const QModelIndex &idx) const
{
    return idx.isValid() && idx.column() < columnCount() && idx.row() < rowCount();
}

const QColor ChatMessageModel::getRandomColor()
{
    if(random_color_list_.size() == 0) {
        random_color_list_.append(QColor("cyan"));
        random_color_list_.append(QColor("magenta"));
        random_color_list_.append(QColor("red"));
        random_color_list_.append(QColor("darkRed"));
        random_color_list_.append(QColor("darkCyan"));
        random_color_list_.append(QColor("darkMagenta"));
        random_color_list_.append(QColor("green"));
        random_color_list_.append(QColor("darkGreen"));
        random_color_list_.append(QColor("yellow"));
        random_color_list_.append(QColor("blue"));
    }

    int idx = qrand() % random_color_list_.size();
    QColor clr(random_color_list_[idx]);
    random_color_list_.removeAt(idx);
    return clr;
}

} // namespace App
} // namespace Web
