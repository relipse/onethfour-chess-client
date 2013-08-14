#include "iccclient.h"

using namespace ICC;

IccClient::IccClient(QObject *parent, const QString& username, const QString& password)
    : ChessClient(parent, Client::Latin1), m_username(username), m_password(password)
{
    m_level2settings = "11111111111111111111111111111111111111111111";
}

void IccClient::on_connected()
{
    //send level2settings
    send(QString(tr("level2settings=")+m_level2settings+tr("\n")).toLatin1());
    if (!m_username.isEmpty()){
        send(QString(m_username + "\n").toLatin1());
        if (!m_password.isEmpty()){
            send(QString(m_password + "\n").toLatin1());
        }
    }else{
        //no username provided, what now?
    }
}

void IccClient::on_disconnected()
{
    //do nothing
}

#include <QRegExp>
void IccClient::on_receiveText(const QString &data)
{
    QString _Y(ICC::DGSPLIT);

    //parse incoming text
    QRegExp rx(_Y + tr("\\((\\d+) (.*?)")+ _Y + tr("\\)"));
    int pos = 0;    // where we are in the string
    int count = 0;  // how many Eric and Eirik's we've counted
    while (pos >= 0) {
        pos = rx.indexIn(str, pos);
        if (pos >= 0) {
            parseDatagram(rx.cap(1).toInt(), rx.cap(0));
            emit onDatagram(rx.cap(1).toInt(), rx.cap(0));
            ++pos;      // move along in str
            ++count;    // count our Eric or Eirik
        }
    }

}

//Parse known datagrams
//todo: we can modify this depending on level2settings since level2settings tells us what
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
        emit onLoggedIn(unparsedDg);
    }else if (dg == DG_PERSONAL_TELL){
        emit onTell("","");
    }else if (dg == DG_LOGIN_FAILED){
        emit onLoginFailed();
    }else{
        //not specifically parsed emit this so clients can parse it
        emit onUnparsedDatagram(dg, unparsedDg);
        return;
    }
    //if execution goes here, that means it successfully parsed and emitted the proper signal
    emit onParseSuccessDatagram(dg, unparsedDg);
}

