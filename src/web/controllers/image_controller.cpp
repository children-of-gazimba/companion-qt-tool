#include "image_controller.h"

#include <QDebug>
#include <QList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUuid>
#include <QFile>

namespace Web {

const QString ImageController::toString(ImageController::FunctionIndex idx)
{
    QString str = "";
    switch(idx) {
        case GET_IMAGE:
            str = "get_image"; break;
        default:
            break;
    }
    return str;
}

const QByteArray ImageController::toByteArray(ImageController::FunctionIndex idx)
{
    return QByteArray(toString(idx).toStdString().c_str());
}

ImageController::FunctionIndex ImageController::toIndex(const QString &str)
{
    if(str.compare("get_image") == 0)
        return GET_IMAGE;
    else
        return NONE;
}

ImageController::FunctionIndex ImageController::toIndex(const QByteArray &arr)
{
    return toIndex(QString(arr));
}

ImageController::ImageController(QObject* parent)
    : stefanfrings::HttpRequestHandler(parent)
    , image_file_name_()
{}

void ImageController::service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response)
{
    QMultiMap<QByteArray,QByteArray> parameters = request.getParameterMap();

    if(parameters.size() == 0) {
        respondNotImplemented(response);
    } else if(parameters.contains(toByteArray(GET_IMAGE))) {
        respondImage(response);
    } else {
        respondNotImplemented(response);
    }
}

void ImageController::onNewImageLoaded(const QString& file_name)
{
    image_file_name_ = file_name;
}

void ImageController::respondNotImplemented(stefanfrings::HttpResponse &response)
{
    Response r("NOT IMPLEMENTED (IMAGE)", QJsonValue(""), "");
    writeAndLog(r, response, true);
}

void ImageController::respondImage(stefanfrings::HttpResponse &response)
{
    if(image_file_name_.size() > 0) {
        QFile* file = new QFile(image_file_name_);
        file->open(QIODevice::ReadOnly);
        QByteArray image = file->readAll();
        response.write(image, true);
    }
    else {
        QJsonObject obj;
        obj["success"] = false;
        Response r ("", QJsonValue(obj), toString(GET_IMAGE));
        writeAndLog(r, response, true);
    }
}

void ImageController::writeAndLog(const Response &response_msg, stefanfrings::HttpResponse &response, bool reply_state)
{
    QByteArray reply = response_msg.toByteArray();
    /*qDebug() << "=========================";
    qDebug() << "RESPONSE SENT.";
    qDebug() << " > " << reply;*/
    response.setHeader("Access-Control-Allow-Origin", "*");
    response.write(reply, reply_state);
}

} // namespace Web

