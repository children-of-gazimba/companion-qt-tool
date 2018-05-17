#include "companion_udp_discovery.h"

#include "network_message.h"

CompanionUdpDiscovery::CompanionUdpDiscovery(unsigned discovery_port, QObject *parent)
    : UdpBroadcastServer(parent)
    , discovery_port_(discovery_port)
{
}

void CompanionUdpDiscovery::broadcast()
{
    QJsonObject o;
    o["port"] = QString::number(discovery_port_);
    NetworkMessage msg("I serve Companion", o);
    qDebug() << msg.toByteArray();
    socket_->writeDatagram(msg.toByteArray(), QHostAddress::Broadcast, port_);
}
