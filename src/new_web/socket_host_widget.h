#ifndef SOCKET_HOST_WIDGET_H
#define SOCKET_HOST_WIDGET_H

#include <QWidget>
#include <QTextEdit>

#include "companion_server.h"
#include "companion_udp_discovery.h"

class SocketHostWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SocketHostWidget(Tile::GraphicsView* view, QWidget *parent = nullptr);

signals:

public slots:

private:
    void initLayout();

    CompanionServer* server_;
    CompanionUdpDiscovery* udp_dicovery_;
    QTextEdit* text_edit_;
};

#endif // SOCKET_HOST_WIDGET_H
