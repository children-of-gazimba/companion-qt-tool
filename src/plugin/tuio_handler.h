#ifndef TUIO_HANDLER_H
#define TUIO_HANDLER_H

#include "plugin/plugin_handler.h"

class QTouchDevice;

class TuioHandler : public PluginHandler
{
    Q_OBJECT
public:
    explicit TuioHandler(QObject *parent = nullptr);
    virtual ~TuioHandler();

    /*
    * Returns the TUIO touch device created by loading this plugin.
    * Or 0 if loading failed.
    */
    static const QTouchDevice* getTuioDevice();

    void setLoggingActivated(bool state);

signals:

protected:
    virtual void loadPlugin();

    QObject* touch_handler_;
};

#endif // TUIO_HANDLER_H
