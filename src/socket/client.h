#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
class QTcpSocket;

class Client : public QObject
{
    Q_OBJECT
    public:
    enum ByteType{Latin1, Utf8};
    explicit Client(QObject *parent = 0, ByteType byteType = Latin1);
    void connect(const QString& host, int port);
    void send(const char *buffer);
    void send(const QString &sbuffer);
public slots:
    //when the client gets connected to the server
    virtual void on_connected();

    virtual void on_error(QAbstractSocket::SocketError error);
    //when disconnected from server
    virtual void on_disconnected();

    //when data gets sent from server to client
    virtual void on_readyread();
    //on_readyread() will call on_receive()
    virtual void on_receiveText(const QString& data);


    protected:
    QTcpSocket* socket;

    private:
    ByteType m_byteType;
};

#endif // CLIENT_H
