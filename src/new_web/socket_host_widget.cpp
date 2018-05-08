#include "socket_host_widget.h"

#include <QHBoxLayout>

SocketHostWidget::SocketHostWidget(Tile::GraphicsView* view, QWidget *parent)
    : QWidget(parent)
    , server_(0)
    , text_edit_(0)
{
    server_ = new CompanionServer(41736, this);
    server_->setGraphicsView(view);

    text_edit_ = new QTextEdit(this);
    text_edit_->setReadOnly(true);
    connect(server_, &CompanionServer::messageSent,
            [=](const QByteArray& arr)
        {
            text_edit_->setText(QString(arr));
        }
    );

    initLayout();
}

void SocketHostWidget::initLayout()
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(text_edit_);
    setLayout(layout);
    setWindowTitle("Companion Server");
}
