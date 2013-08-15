#include "iccclient.h"
#include "chessclient.h"

#include <QRegExp>
#include <QDebug>

using namespace ICC;

IccClient::IccClient(QObject *parent, const QString& username, const QString& password)
    : ChessClient(parent, Client::Latin1),
      m_username(username), m_password(password),
      m_loggedInHandle(""), m_loggedInTitles(""), m_lastTellFrom("")
{
    m_level2settings = "11111111111111111111111111111111111111111111";
}

void IccClient::on_connected()
{
    //send level2settings
    send(tr("level2settings=")+m_level2settings+tr("\n"));
    if (!m_username.isEmpty()){
        send(QString(m_username + "\n"));
        if (!m_password.isEmpty()){
            send(QString(m_password + "\n"));
        }
    }else{
        //no username provided, what now?
    }
}

void IccClient::on_error(QAbstractSocket::SocketError error)
{
    emit onError(error);
    qDebug() << "Error: " << error;
}

void IccClient::on_disconnected()
{
    //do nothing
}


void IccClient::on_receiveText(const QString &data)
{
    emit onData(data);
    //Examples below:
    //(32 CHENNAIBOND {IM} 1 {wanted to play Rxd8})aics%
    //(32 knightrunner {} 1 {Sir H!})
    //QString regex = "\\((\\d+) (.+)\\)|([^]+)";
    QString regex = "\\((\\d+) (.+)\\)|([^]+)";
    //QRegExp does not allow one paren to be greedy and another to not be greedy
    //TODO: convert this to Qt5 and use QRegularExpression
    QRegExp rx(regex);
    rx.setMinimal(true);
    int pos = rx.indexIn(data);    // where we are in the string
    int count = 0;  // how many dgs have been sent
    if (pos == -1){
        //there were no dgs in the text, just send as server text
        emit onNonDatagram(data);

        //TODO: what do we do with plain text in between a datagram?
        //TODO: or is this even possible?
    }
    while (pos >= 0) {
        pos = rx.indexIn(data, pos);
        if (pos >= 0) {
            qDebug() << rx.cap(1) << rx.cap(0) << rx.cap(3);
            if (!rx.cap(1).isEmpty())
            {
                parseDatagram(rx.cap(1).toInt(), rx.cap(0));
                emit onDatagram(rx.cap(1).toInt(), rx.cap(0));
            }else if (!rx.cap(3).isEmpty()){
                //not a datagram
                emit onNonDatagram(rx.cap(3));
            }
            ++pos;      // move along in str
            ++count;
        }
    }
}



//Parse known datagrams
//TODO: we can modify this depending on level2settings since level2settings tells us what
//type of dgs will be sent
void IccClient::parseDatagram(int dg, const QString &unparsedDg)
{
    QString dgLabel(ICC::DatagramToString(dg));
    qDebug() << dgLabel << " received: " << unparsedDg;

    /* TODO: figure this out
    if (emit onBeforeParseDatagram(dg, unparsedDg) == false){
        return;
    }*/

    if (dg == DG_WHO_AM_I){
        //(0 TexasHoldem {C})
        QRegExp rx("\\((\\d+) (.+) \\{(.*)\\}\\)");
        rx.setMinimal(true);
        rx.indexIn(unparsedDg);
        m_loggedInHandle = rx.cap(2);
        m_loggedInTitles = rx.cap(3);
        emit onLoggedIn(m_loggedInHandle, m_loggedInTitles);
    }else if (dg == DG_PERSONAL_TELL){
        ////(32 knightrunner {} 1 {Sir H!})
        QRegExp rx("\\((\\d+) (.+) \\{(.*)\\} (\\d+) \\{(.*)\\}(.*)\\)");
        rx.setMinimal(true);
        if (rx.indexIn(unparsedDg) == -1){
            qDebug() << "error parsing a tell, either the regexp is wrong or they changed formats";
        }else{
            QString fromHandle = rx.cap(2);
            m_lastTellFrom = fromHandle;
            QString telltext = rx.cap(5);
            emit onTell(fromHandle, rx.cap(3), rx.cap(4).toInt(), telltext);
        }
    }else if (dg == DG_LOGIN_FAILED){
        emit onLoginFailed();
    }else{
        //TODO: finish parsing the rest
        //not specifically parsed emit this so clients can parse it
        emit onUnparsedDatagram(dg, unparsedDg);
        return;
    }
    //if execution goes here, that means it successfully parsed and emitted the proper signal
    emit onParseSuccessDatagram(dg, unparsedDg);
}

void IccClient::SetUsername(const QString &username)
{
    m_username = username;
}

void IccClient::SetPassword(const QString &password)
{
    m_password = password;
}

const QString &IccClient::loggedInHandle()
{
    return m_loggedInHandle;
}

const QString &IccClient::lastTellFrom()
{
    return m_lastTellFrom;
}

