#include "chat.h"

#include <QHBoxLayout>
#include <QDebug>

namespace Web {
namespace App {

Chat::Chat(ChatMessageModel* model, QWidget *parent)
    : QWidget(parent)
    , text_edit_(0)
    , line_edit_(0)
    , model_(model)
{
    initWidgets();
    initLayout();
}

void Chat::onReturnPressed()
{
    ChatMessage cm("Master", line_edit_->text());
    model_->addMessage(cm);
    line_edit_->clear();
}

void Chat::onModelMessageAdded(const ChatMessage msg)
{
    QColor clr = model_->getSenderColor(msg);
    QString str = "";
    str += "<html><b><big><font color=\"" + clr.name() + "\">" + msg.from + "</font></big></b>";
    str += " <small>[" + msg.time_stamp.toString() + "]</small>: ";
    str += "<big>" + msg.content + "</big><br></html>";
    text_edit_->moveCursor(QTextCursor::End);
    text_edit_->insertHtml(str);
}

void Chat::initWidgets()
{
    text_edit_ = new QTextEdit(this);
    text_edit_->setReadOnly(true);

    line_edit_ = new QLineEdit(this);

    connect(line_edit_, SIGNAL(returnPressed()),
            this, SLOT(onReturnPressed()));
    qRegisterMetaType<Web::App::ChatMessage>("Web::App::ChatMessage");
    connect(model_, SIGNAL(messageAdded(const Web::App::ChatMessage)),
            this, SLOT(onModelMessageAdded(const Web::App::ChatMessage)),
            Qt::QueuedConnection);
}

void Chat::initLayout()
{
    QVBoxLayout* layout = new QVBoxLayout;

    layout->addWidget(text_edit_, 10);
    layout->addWidget(line_edit_, 0);

    setLayout(layout);
}

} // namespace App
} // namespace Web
