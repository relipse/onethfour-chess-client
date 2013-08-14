#ifndef CHESSCLIENT_H
#define CHESSCLIENT_H

#include "client.h"
class ChessClient : public Client
{
    Q_OBJECT
public:
    explicit ChessClient(QObject *parent = 0, ByteType byteType = Latin1);

    public slots:
    //when the client gets connected to the server
    virtual void on_connected();

    //when disconnected from server
    virtual void on_disconnected();

    //on_readyread() will call on_receive()
    virtual void on_receiveText(const QString& data);
};

#endif // CHESSCLIENT_H
