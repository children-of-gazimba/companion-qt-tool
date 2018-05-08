#include "companion_server.h"

CompanionServer::CompanionServer(int port, QObject *parent)
    : Server(port, parent)
    , view_(0)
{}

CompanionServer::~CompanionServer()
{
    foreach(auto c, clients_)
        c->close();
}

void CompanionServer::setGraphicsView(Tile::GraphicsView *view)
{
    view_ = view;
}

void CompanionServer::newConnection()
{
    qDebug().nospace() << Q_FUNC_INFO << " @ line " << __LINE__;
    qDebug() << "  > " << "new incoming connection";
    // need to grab the socket
    QTcpSocket *socket = tcp_server_->nextPendingConnection();

    if(socket->bytesAvailable())
        qDebug() << "    > "  << "Client says:" << socket->readAll();

    if(view_) {
        QJsonDocument doc(view_->toJsonObject());
        QByteArray data(doc.toJson());
        socket->write(data);
        socket->flush();
        emit messageSent(data);
    }
    clients_.push_back(socket);

    connect(socket, &QTcpSocket::disconnected,
            [=]()
        {
            qDebug() << "client disconnected";
            socket->close();
            clients_.removeAll(socket);
        }
    );
}

void CompanionServer::initServer()
{
    tcp_server_ = new QTcpServer(this);

    // whenever a user connects, it will emit signal
    connect(tcp_server_, SIGNAL(newConnection()),
            this, SLOT(newConnection()));

    if(!tcp_server_->listen(QHostAddress::Any, port_))
    {
        qDebug().nospace() << Q_FUNC_INFO << " @ line " << __LINE__;
        qDebug() << "  > " << "Failure: Server couldn't start";
    }
    else
    {
        qDebug().nospace() << Q_FUNC_INFO << " @ line " << __LINE__;
        qDebug() << "  > " << "Success: Server started.";
    }

    refresh_read_ = new QTimer(this);
}
