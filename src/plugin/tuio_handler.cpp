#include "tuio_handler.h"

#include <QtGui/QGenericPlugin>
#include <QTouchDevice>
#include <QDebug>
#include <QLoggingCategory>

#include "resources/lib.h"

TuioHandler::TuioHandler(QObject *parent)
    : PluginHandler(Resources::Lib::TUIO_PLUGIN_DIR, "QTuioTouchPlugin", parent)
    , touch_handler_(0)
{}

TuioHandler::~TuioHandler()
{
    if(touch_handler_)
        touch_handler_->deleteLater();
}

const QTouchDevice *TuioHandler::getTuioDevice()
{
    foreach(auto dev, QTouchDevice::devices()) {
        if(dev->name().compare("TUIO") == 0)
            return dev;
    }
    return 0;
}

void TuioHandler::setLoggingActivated(bool state)
{
    if(state)
        QLoggingCategory::setFilterRules("qt.qpa.tuio.*=true");
    else
        QLoggingCategory::setFilterRules("qt.qpa.tuio.*=false");
}

void TuioHandler::loadPlugin()
{
    PluginHandler::loadPlugin();
    if(plugin_) {
        touch_handler_ = plugin_->create("TuioTouch", "udp=3333");
        qDebug() << "       > device" << getTuioDevice();
    }
}
