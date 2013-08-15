#include "client.h"
#include <QTcpSocket>
#include <QHostAddress>

Client::Client(QObject *parent, ByteType byteType)
    : QObject(parent), m_byteType(byteType)
{
    socket = new QTcpSocket(this);
     QObject::connect(socket, SIGNAL(connected()), this, SLOT(on_connected()));
     QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(on_readyread()));
     QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(on_error(QAbstractSocket::SocketError)));
     QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
}

void Client::on_connected()
{
    //do nothing
}

void Client::on_error(QAbstractSocket::SocketError error)
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

void Client::send(const QString& sbuffer)
{
    if (m_byteType == Latin1){
        socket->write(sbuffer.toLatin1());
    }else if (m_byteType == Utf8){
        socket->write(sbuffer.toUtf8());
    }
    socket->flush();
}

void Client::send(const char* buffer){
    socket->write(buffer);
    socket->flush();
}

void Client::connect(const QString &host, int port)
{
    socket->connectToHost(host, port);
}
