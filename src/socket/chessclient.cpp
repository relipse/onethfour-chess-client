#include "chessclient.h"

ChessClient::ChessClient(QObject *parent, ByteType byteType)
    : Client(parent, byteType)
{
    //do nothing
}

void ChessClient::on_connected()
{
    //do nothing
}

void ChessClient::on_error(QAbstractSocket::SocketError error)
{
    //do nothing
}

void ChessClient::on_disconnected()
{
    //do nothing
}

void ChessClient::on_receiveText(const QString &data)
{
    //we need to parse the data received, but wait for class to inherit this one
}


