#include "chat_controller.h"

#include <QDebug>
#include <QList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUuid>

namespace Web {

const QString ChatController::toString(ChatController::FunctionIndex idx)
{
    QString str = "";
    switch(idx) {
        case MESSAGE:
            str = "message"; break;
        default:
            break;
    }
    return str;
}

const QByteArray ChatController::toByteArray(ChatController::FunctionIndex idx)
{
    return QByteArray(toString(idx).toStdString().c_str());
}

ChatController::FunctionIndex ChatController::toIndex(const QString &str)
{
    if(str.compare("message") == 0)
        return MESSAGE;
    else
        return NONE;
}

ChatController::FunctionIndex ChatController::toIndex(const QByteArray &arr)
{
    return toIndex(QString(arr));
}

ChatController::ChatController(QObject* parent)
    : stefanfrings::HttpRequestHandler(parent)
    , message_model_(0)
{
    message_model_ = new App::ChatMessageModel(this);
}

App::ChatMessageModel *ChatController::getModel()
{
    return message_model_;
}

void ChatController::service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response)
{
    QMultiMap<QByteArray,QByteArray> parameters = request.getParameterMap();

    if(parameters.size() == 0) {
        respondNotImplemented(response);
    } else if(parameters.contains(toByteArray(MESSAGE))) {
        parseMessageRequest(parameters, response);
    } else {
        respondNotImplemented(response);
    }
}

void ChatController::respondNotImplemented(stefanfrings::HttpResponse &response)
{
    Response r("NOT IMPLEMENTED (CHAT)", QJsonValue(""), "");
    writeAndLog(r, response, true);
}

void ChatController::parseMessageRequest(const QMultiMap<QByteArray, QByteArray> &parameters, stefanfrings::HttpResponse &response)
{
    QByteArray message_from = parameters.value(toByteArray(MESSAGE));
    QByteArray message_content = parameters.value("content");

    Web::App::ChatMessage cm;
    cm.from = QString(message_from);
    cm.content = QString(message_content);
    message_model_->addMessage(cm);

    QJsonObject obj;
    obj["success"] = true;
    Response r ("", QJsonValue(obj), toString(MESSAGE));

    writeAndLog(r, response, true);
}

void ChatController::writeAndLog(const Response &response_msg, stefanfrings::HttpResponse &response, bool reply_state)
{
    QByteArray reply = response_msg.toByteArray();
    /*qDebug() << "=========================";
    qDebug() << "RESPONSE SENT.";
    qDebug() << " > " << reply;*/
    response.setHeader("Access-Control-Allow-Origin", "*");
    response.write(reply, reply_state);
}

} // namespace Web

