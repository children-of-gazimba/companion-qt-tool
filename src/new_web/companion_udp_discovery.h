#ifndef COMPANION_UDP_DISCOVERY_H
#define COMPANION_UDP_DISCOVERY_H

#include <QObject>

#include "udp_broadcast_server.h"

class CompanionUdpDiscovery : public UdpBroadcastServer
{
    Q_OBJECT
public:
    explicit CompanionUdpDiscovery(unsigned discovery_port, QObject *parent = nullptr);

signals:

public slots:

protected slots:
    virtual void broadcast();

private:
    unsigned discovery_port_;
};

#endif // COMPANION_UDP_DISCOVERY_H
