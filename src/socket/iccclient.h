#ifndef ICCCLIENT_H
#define ICCCLIENT_H

#include <QObject>
#include <QTcpSocket>
class QTcpSocket;

#include "../icc/icc.h"

#include "chessclient.h"

class IccClient : public ChessClient
{
    Q_OBJECT
    public:
    enum ByteType{Latin1, Utf8};
    explicit IccClient(QObject *parent = 0, const QString &username="", const QString &password="");

    public slots:
    //when the client gets connected to the server
    virtual void on_connected();

    virtual void on_error(QAbstractSocket::SocketError error);

    //when disconnected from server
    virtual void on_disconnected();

    //on_readyread() will call on_receive()
    virtual void on_receiveText(const QString& data);

public:
    virtual void parseDatagram(int dg, const QString& unparsedDg);

   signals:
    void onError(QAbstractSocket::SocketError error);

    void onData(const QString& unparsedData);

    //this happens on plaintext that doesnt match the datagram pattern
    void onNonDatagram(const QString& text);
    //this happens on every datagram passed
    void onDatagram(int dg, const QString& unparsedDg);

    //this happens before every datagram gets parsed and if return false then stops the parsing
    bool onBeforeParseDatagram(int dg, const QString& unparsedDg);

    //this depends on parseDatagram if it parses the datagram and emits another signal
    void onUnparsedDatagram(int dg, const QString& unparsedDg);

    //after it runs through parseDatagram and succeeds, it will emit this signal
    void onParseSuccessDatagram(int dg, const QString& unparsedDg);

    //these happen for specific datagrams parsed to make things a bit easier for programmers
    void onLoggedIn(const QString& myHandle, const QString& myTitles);
    void onLoginFailed();
    void onTell(const QString& fromHandle, const QString& fromTitles, int type, const QString& text);

public:
    void SetUsername(const QString& username);
    void SetPassword(const QString& password);

    const QString&  loggedInHandle();
    const QString&  lastTellFrom();

protected:
    QString m_level2settings;
    QString m_username;
    QString m_password;

    QString m_loggedInHandle;
    QString m_loggedInTitles;

    QString m_lastTellFrom;
};

#endif // ICCCLIENT_H
