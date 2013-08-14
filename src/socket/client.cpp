#include "client.h"
#include <QTcpSocket>
#include <QHostAddress>

Client::Client(QObject *parent, ByteType byteType)
    : QObject(parent), m_byteType(byteType)
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(on_connected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(on_readyread()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
}

void Client::on_connected()
{
    //do nothing
}

void Client::on_disconnected()
{
    //do nothing
}

//read all data from server
//process depending on type
void Client::on_readyread()
{
    QString data;
    QByteArray bytes = socket->readAll();
    if (m_byteType == Latin1){
        data = QString::fromLatin1(bytes);
    }else if (m_byteType == Utf8){
        data = QString::fromUtf8(bytes);
    }
    this->on_receiveText(data);
}

void Client::on_receiveText(const QString &data)
{
    //do something
}

void Client::send(char* buffer){
    socket->write(buffer);
    socket->flush();
}

void Client::connect(const QHostAddress& host, int port)
{
    socket->connectToHost(host, port);
}