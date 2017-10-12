#ifndef WEB_APP_CHAT_H
#define WEB_APP_CHAT_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QTableView>

#include "chat_message_model.h"

namespace Web {
namespace App {

class Chat : public QWidget
{
    Q_OBJECT
public:
    explicit Chat(ChatMessageModel* model, QWidget *parent = 0);

signals:

public slots:
private slots:
    void onReturnPressed();
    void onModelMessageAdded(const Web::App::ChatMessage msg);

private:
    void initWidgets();
    void initLayout();

    QTextEdit* text_edit_;
    QLineEdit* line_edit_;
    ChatMessageModel* model_;
};

} // namespace App
} // namespace Web

#endif // WEB_APP_CHAT_H
