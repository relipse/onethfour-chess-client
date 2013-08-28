#include "iccclient.h"
#include "chessclient.h"

#include <QRegExp>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
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

    //use a raw string literal so we dont have to double escape for the regex
    //QString regex = R"x(\((\d+) (.+)\)|([^]+))x";
    QString regex = "\\((\\d+) (.+?)\\)|([^]+)";
    qDebug() << regex;

    QRegularExpression rx(regex);
    QRegularExpressionMatchIterator i  = rx.globalMatch(data);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        if (!match.captured(1).isEmpty())
        {
            parseDatagram(match.captured(1).toInt(), match.captured(0));
            emit onDatagram(match.captured(1).toInt(), match.captured(0));
        }else{ //matched 3
            emit onNonDatagram(match.captured(3));
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
        if (!m_interface.isEmpty()){
            send(tr("set interface ")+m_interface + "\n");
        }
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
    }else if (dg == DG_MY_GAME_STARTED){
        IccDgGameStarted dgGameStarted;

        if (parseDgGameStarted(unparsedDg, dgGameStarted)){
            emit onMyGameStarted(dgGameStarted);
        }else{
            qDebug() << "error parsing";
        }

    }else if (dg == DG_GAME_STARTED){
        //note: this is not enabled unless set-level[12] is 1 and then the person
        //will see all of the games that get started
        IccDgGameStarted dgGameStarted;
        if (parseDgGameStarted(unparsedDg, dgGameStarted)){
            emit onGameStarted(dgGameStarted);
        }else{
            qDebug() << "error parsing";
        }
    }else if (dg == DG_STARTED_OBSERVING){
        IccDgGameStarted dgStartedObserving;
        if (parseDgGameStarted(unparsedDg, dgStartedObserving)){
            emit onStartedObserving(dgStartedObserving);
        }
        /******
         *DG_SHOUT(32 Flash {TD} 2 {snowden won the SPEEEEEED Luton Bullet 0 1 rated manager lani123 tournament, with a score 6/7!})
aics%
DG_STATE(11 aggressor E 172)
Added (and emailed) the following message to suggestion:
--relipse (21:14 27-Aug-13 EDT): Fics has a position search, can ICC please implement that? would be very cool to check out games in my openings from high rated players
aics%
You are now observing game 464.
DG_STARTED_OBSERVING(18 464 Mashin khallod 0 1-minute 1 1 0 1 0 1 {} 2132 2088 1625607086 {IM} {IM} 0 0 0 {} 0)
DG_MY_RELATION_TO_GAME(43 464 O)
DG_PLAYERS_IN_MY_GAME(20 464 relipse O 1)
DG_PLAYERS_IN_MY_GAME(20 464 samod1 O 1)
DG_PLAYERS_IN_MY_GAME(20 464 shadodreamz O 1)
DG_PLAYERS_IN_MY_GAME(20 464 DeMarco O 1)
DG_PLAYERS_IN_MY_GAME(20 464 hadarpaz O 1)
DG_PLAYERS_IN_MY_GAME(20 464 salta O 1)
DG_PLAYERS_IN_MY_GAME(20 464 Qwellan O 1)
DG_PLAYERS_IN_MY_GAME(20 464 Mashin PW 1)
DG_PLAYERS_IN_MY_GAME(20 464 khallod PB 1)
DG_FLIP(39 464 0)
DG_MOVE_LIST(25 464 * {g3 g2g3 1 59}{d5 d7d5 1 59}{Bg2 f1g2 0 59}{Nf6 g8f6 1 58}{Nf3 g1f3 0 58}{g6 g7g6 0 58}{O-O e1g1c 0 58}{Bg7 f8g7 0 58}{b3 b2b3 0 58}{c6 c7c6 0 58}{Bb2 c1b2 0 57}{O-O e8g8c 1 57}{d3 d2d3 0 57}{Qa5 d8a5 1 56}{Nbd2 b1d2 1 56}{Bg4 c8g4 1 55}{a3 a2a3 1 55}{Nbd7 b8d7 3 52}{b4 b3b4 1 54}{Qc7 a5c7 1 51}{c4 c2c4 0 54}{e6 e7e6 2 49}{Rc1 a1c1 1 53}{Rfe8 f8e8 1 48}{h3 h2h3 1 52}{Bxf3 g4f3n 2 46}{Nxf3 d2f3b 0 52}{Rad8 a8d8 2 44}{Qc2 d1c2 1 51}{Qb8 c7b8 1 43}{Rfd1 f1d1 1 50}{e5 e6e5 1 42}{Qb1 c2b1 1 49}{e4 e5e4 2 41}{Nd4 f3d4 1 49}{e3 e4e3 2 39}{f4 f2f4 3 46}{Nh5 f6h5 0 39}{Kh2 g1h2 1 45}{dxc4 d5c4p 1 38}{dxc4 d3c4p 2 43}{Ndf6 d7f6 2 36}{Nc2 d4c2 4 39}{Nxg3 h5g3p 1 35}{Kxg3 h2g3n 1 38}{Nh5+ f6h5 1 33}{Kh2 g3h2 2 36}{Qxf4+ b8f4p 0 33}{Kg1 h2g1 1 36}{Qf2+ f4f2 1 32}{Kh1 g1h1 1 35}{Ng3+ h5g3 3 28}{Kh2 h1h2 1 34}{Nxe2 g3e2p 0 28}{Bxg7 b2g7b 7 27}{Qf4+ f2f4 3 25}{Kh1 h2h1 2 25}{Kxg7 g8g7b 0 25}{Rf1 d1f1 4 21}{Ng3+ e2g3 2 23}{Kg1 h1g1 0 21}{Nxf1 g3f1r 1 22}{Rxf1 c1f1n 0 21}{Qe5 f4e5 4 18}{Qe1 b1e1 1 20}{e2 e3e2 2 16}{Rf2 f1f2 2 17}{Rd1 d8d1 1 16}{Bf3 g2f3 1 16}{Rxe1+ d1e1q 1 14}{Nxe1 c2e1r 0 16}{Qg3+ e5g3 0 14}{Bg2 f3g2 1 15}{f5 f7f5 2 12}{Kh1 g1h1 1 15}{f4 f5f4 0 12}{Kg1 h1g1 1 14}{f3 f4f3 1 11}{Nxf3 e1f3p 2 11}{e1=Q+ e2e1Q 1 10}{Nxe1 f3e1q 1 11}{Rxe1+ e8e1n 0 10}{Rf1 f2f1 0 10}{Qe3+ g3e3 1 10}{Kh2 g1h2 1 9}{Rxf1 e1f1r 0 10}{Bxf1 g2f1r 0 9}{Qf2+ e3f2 0 9}{Bg2 f1g2 1 8}{g5 g6g5 0 9}{Kh1 h2h1 1 7}{Kf6 g7f6 0 9}{b5 b4b5 1 7})
DG_OFFERS_IN_MY_GAME(21 464 0 0 0 0 0 0 0 0)
DG_SET_CLOCK(38 464 7 9)
Game 464 (Mashin vs. khallod)

    ---------------------------------
     8  |   |   |   |   |   |   |   |   |     Move # : 47 (Black)
    |---+---+---+---+---+---+---+---|
     7  | *P| *P|   |   |   |   |   | *P|
  White Moves : 'b5 (0:01)'
    |---+---+---+---+---+---+---+---|
     6  |   |   | *P|   |   | *K|   |   |
    |---+---+---+---+---+---+---+---|
     5  |   | P |   |   |   |   | *P|   |     Black Clock : 0 : 09
    |---+---+---+---+---+---+---+---|
     4  |   |   | P |   |   |   |   |   |     White Clock : 0 : 07
    |---+---+---+---+---+---+---+---|
     3  | P |   |   |   |   |   |   | P |     Black Strength : 14
    |---+---+---+---+---+---+---+---|
     2  |   |   |   |   |   | *Q| B |   |     White Strength : 7
    |---+---+---+---+---+---+---+---|
     1  |   |   |   |   |   |   |   | K |
    ---------------------------------
      a   b   c   d   e   f   g   h
aics%

Game 464 (Mashin vs. khallod)
DG_SEND_MOVES(24 464 Ke5 f6e5 0 9)


    ---------------------------------
     8  |   |   |   |   |   |   |   |   |     Move # : 48 (White)
    |---+---+---+---+---+---+---+---|
     7  | *P| *P|   |   |   |   |   | *P|     Black Moves : 'Ke5 (0:00)'
    |---+---+---+---+---+---+---+---|
     6  |   |   | *P|   |   |   |   |   |
    |---+---+---+---+---+---+---+---|
     5  |   | P |   |   | *K|   | *P|   |     Black Clock : 0 : 09
    |---+---+---+---+---+---+---+---|
     4  |   |   | P |   |   |   |   |   |     White Clock : 0 : 07
    |---+---+---+---+---+---+---+---|
     3  | P |   |   |   |   |   |   | P |     Black Strength : 14
    |---+---+---+---+---+---+---+---|
     2  |   |   |   |   |   | *Q| B |   |     White Strength : 7
    |---+---+---+---+---+---+---+---|
     1  |   |   |   |   |   |   |   | K |
    ---------------------------------
      a   b   c   d   e   f   g   h
aics%

Game 464 (Mashin vs. khallod)
DG_SEND_MOVES(24 464 bxc6 b5c6p 1 6)


    ---------------------------------
     8  |   |   |   |   |   |   |   |   |     Move # : 48 (Black)
    |---+---+---+---+---+---+---+---|
     7  | *P| *P|   |   |   |   |   | *P|     White Moves : 'bxc6 (0:01)'
    |---+---+---+---+---+---+---+---|
     6  |   |   | P |   |   |   |   |   |
    |---+---+---+---+---+---+---+---|
     5  |   |   |   |   | *K|   | *P|   |     Black Clock : 0 : 09
    |---+---+---+---+---+---+---+---|
     4  |   |   | P |   |   |   |   |   |     White Clock : 0 : 06
    |---+---+---+---+---+---+---+---|
     3  | P |   |   |   |   |   |   | P |     Black Strength : 13
    |---+---+---+---+---+---+---+---|
     2  |   |   |   |   |   | *Q| B |   |     White Strength : 7
    |---+---+---+---+---+---+---+---|
     1  |   |   |   |   |   |   |   | K |
    ---------------------------------
      a   b   c   d   e   f   g   h
aics%

Game 464 (Mashin vs. khallod)
DG_SEND_MOVES(24 464 bxc6 b7c6p 1 8)


    ---------------------------------
     8  |   |   |   |   |   |   |   |   |     Move # : 49 (White)
    |---+---+---+---+---+---+---+---|
     7  | *P|   |   |   |   |   |   | *P|     Black Moves : 'bxc6 (0:01)'
    |---+---+---+---+---+---+---+---|
     6  |   |   | *P|   |   |   |   |   |
    |---+---+---+---+---+---+---+---|
     5  |   |   |   |   | *K|   | *P|   |     Black Clock : 0 : 08
    |---+---+---+---+---+---+---+---|
     4  |   |   | P |   |   |   |   |   |     White Clock : 0 : 06
    |---+---+---+---+---+---+---+---|
     3  | P |   |   |   |   |   |   | P |     Black Strength : 13
    |---+---+---+---+---+---+---+---|
     2  |   |   |   |   |   | *Q| B |   |     White Strength : 6
    |---+---+---+---+---+---+---+---|
     1  |   |   |   |   |   |   |   | K |
    ---------------------------------
      a   b   c   d   e   f   g   h
aics%

Game 464 (Mashin vs. khallod)
DG_SEND_MOVES(24 464 Bxc6 g2c6p 0 6)


    ---------------------------------
     8  |   |   |   |   |   |   |   |   |     Move # : 49 (Black)
    |---+---+---+---+---+---+---+---|
     7  | *P|   |   |   |   |   |   | *P|     White Moves : 'Bxc6 (0:00)'
    |---+---+---+---+---+---+---+---|
     6  |   |   | B |   |   |   |   |   |
    |---+---+---+---+---+---+---+---|
     5  |   |   |   |   | *K|   | *P|   |     Black Clock : 0 : 08
    |---+---+---+---+---+---+---+---|
     4  |   |   | P |   |   |   |   |   |     White Clock : 0 : 06
    |---+---+---+---+---+---+---+---|
     3  | P |   |   |   |   |   |   | P |     Black Strength : 12
    |---+---+---+---+---+---+---+---|
     2  |   |   |   |   |   | *Q|   |   |     White Strength : 6
    |---+---+---+---+---+---+---+---|
     1  |   |   |   |   |   |   |   | K |
    ---------------------------------
      a   b   c   d   e   f   g   h
aics%

Game 464 (Mashin vs. khallod)
DG_SEND_MOVES(24 464 Kf4 e5f4 0 8)


    ---------------------------------
     8  |   |   |   |   |   |   |   |   |     Move # : 50 (White)
    |---+---+---+---+---+---+---+---|
     7  | *P|   |   |   |   |   |   | *P|     Black Moves : 'Kf4 (0:00)'
    |---+---+---+---+---+---+---+---|
     6  |   |   | B |   |   |   |   |   |
    |---+---+---+---+---+---+---+---|
     5  |   |   |   |   |   |   | *P|   |     Black Clock : 0 : 08
    |---+---+---+---+---+---+---+---|
     4  |   |   | P |   |   | *K|   |   |     White Clock : 0 : 06
    |---+---+---+---+---+---+---+---|
     3  | P |   |   |   |   |   |   | P |     Black Strength : 12
    |---+---+---+---+---+---+---+---|
     2  |   |   |   |   |   | *Q|   |   |     White Strength : 6
    |---+---+---+---+---+---+---+---|
     1  |   |   |   |   |   |   |   | K |
    ---------------------------------
      a   b   c   d   e   f   g   h
aics%
DG_PLAYERS_IN_MY_GAME(20 464 tophop O 1)

{Game 464 (Mashin vs. khallod) Mashin resigns} 0-1
DG_MY_GAME_RESULT(16 464 1 Res 0-1 {White resigns} {A00})
Game 464 becomes an examined game.
DG_PLAYERS_IN_MY_GAME(20 464 khallod E 1)
DG_PLAYERS_IN_MY_GAME(20 464 Mashin X 1)
aics%
DG_MY_GAME_ENDED(17 464)

Game 464 (which you were observing) has no examiners.
DG_PLAYERS_IN_MY_GAME(20 464 hadarpaz X 1)
DG_PLAYERS_IN_MY_GAME(20 464 samod1 X 1)
DG_PLAYERS_IN_MY_GAME(20 464 DeMarco X 1)
DG_PLAYERS_IN_MY_GAME(20 464 tophop X 1)
DG_PLAYERS_IN_MY_GAME(20 464 salta X 1)
DG_PLAYERS_IN_MY_GAME(20 464 Qwellan X 1)
DG_PLAYERS_IN_MY_GAME(20 464 shadodreamz X 1)
DG_MY_RELATION_TO_GAME(43 464 X)
DG_PLAYERS_IN_MY_GAME(20 464 relipse X 1)
DG_STOP_OBSERVING(19 464)***********/


    }else if (dg == DG_MY_RELATION_TO_GAME){
        //DG_MY_RELATION_TO_GAME(43 959 PW)
        QRegularExpression re("\\((?P<dg>\\d+) (?P<gamenum>\\d+) (?P<symbol>\\w+)\\)");
        QRegularExpressionMatch match = re.match(unparsedDg);
        emit onMyRelationToGame(match.captured("gamenum").toInt(), match.captured("symbol"));
    }else if (dg == DG_SEND_MOVES){
        //DG_SEND_MOVES(24 959 e4 e2e4 23 37)
        // (gamenumber algebraic-move smith-move time clock is-variation)
        QRegularExpression re("\\((?P<dg>\\d+) (?P<gamenum>\\d+) (?P<algebraic>[^ ]+) (?P<smith>[^ ]+) (?P<timetook>\\d+)? (?P<clock>\\d+)( (?P<isvariation>\\d))?\\)");
        QRegularExpressionMatch match = re.match(unparsedDg);
        qDebug() << "mached SEND_MOVES? " << match;
        emit onSendMoves(match.captured("gamenum").toLong(), match.captured("algebraic"), match.captured("smith"), match.captured("timetook").toInt(), match.captured("clock").toInt(), match.captured("isvariation").toInt());
    }else if (dg == DG_TAKEBACK){
        //DG_TAKEBACK(22 1541 1)

        QRegularExpression re("\\((?P<dg>\\d+) (?P<gamenum>\\d+) (?P<takebackply>\\d+)\\)");
        QRegularExpressionMatch match = re.match(unparsedDg);
        qDebug() << "emittting a dgTakeBack" ;
        emit onTakebackMove(match.captured("gamenum").toLong(), match.captured("takebackply").toLong());
    }

    else if (dg == DG_ILLEGAL_MOVE){
        QRegularExpression re("\\((?P<dg>\\d+) (?P<gamenum>\\d+)( (?P<movestring>.*?))? (?P<reason>\\d+)\\)");
        QRegularExpressionMatch match = re.match(unparsedDg);

        emit onIllegalMove(match.captured("gamenum").toLong(), match.captured("movestring"), match.captured("reason").toInt());
    }else if (dg == DG_PLAYERS_IN_MY_GAME){
     //do something
    }else if (dg == DG_FLIP){
        QRegularExpression re("\\((?P<dg>\\d+) (?P<gamenum>\\d+) (?P<flip>\\d+)\\)");
        QRegularExpressionMatch match = re.match(unparsedDg);
        emit onFlip(match.captured("gamenum").toLong(), match.captured("flip").toInt());
    }else if (dg == DG_MOVE_LIST){
//        DG_MY_GAME_STARTED(15 1021 relipse TrainingBot 20 Blitz 0 20 10 20 10 1 {} 1827 1600 1624995327 {} {C} 0 0 0 {} 0)
//        DG_MY_RELATION_TO_GAME(43 1021 PW)
//        DG_PLAYERS_IN_MY_GAME(20 1021 relipse PW 1)
//        DG_FLIP(39 1021 0)
//        DG_MOVE_LIST(25 1021 -----B----p------p---k-pq---N---r-----P-------P--P----K---Q----- {Qc6+ c1c6 18 1192}{Kxe5 f6e5n 0 1210})
//        DG_SET_CLOCK(38 1021 1174 1225)
//        DG_PLAYERS_IN_MY_GAME(20 1021 TrainingBot PB 1)



        //(25 893 -----B----p------p---k-pq---N---r-----P-------P--P----K---Q----- )
        QString regex_move_list = "\\((\\d+) (\\d+) (.+?) ?(?<curlybracemoves>(?<move>\\{(.*?)\\})*?)\\)";
        QRegularExpression re(regex_move_list);
        QRegularExpressionMatch match = re.match(unparsedDg);


        QString curlybrace_moves = match.captured("curlybracemoves");
        qDebug() << "unparsed move list: " << curlybrace_moves;

        QList<IccCurlyChessMove> move_list;
        if (curlybrace_moves.isEmpty() == false){
            qDebug() << "parsed move list!";
            QStringList sl;
            QString s = match.captured("curlybracemoves").replace("{", "").trimmed();
            sl = s.split(QRegularExpression("\\}"), QString::SkipEmptyParts);

            int lastMoveNum = 0;
            for (int i = 0; i < sl.count();++i){
                QRegularExpression re2("(?<algebraic>[^\\s]+) (?<fromTo>[^\\s]+) (?<num>\\d+) (?<timeleft>\\d+)");
                QRegularExpressionMatch ma2 = re2.match(sl.at(i));
                qDebug() << "matched curly brace move: " << ma2.captured("algebraic");
                IccCurlyChessMove ccm;
                ccm.algebraic = ma2.captured("algebraic");
                ccm.fromTo = ma2.captured("fromTo");
                ccm.num = ma2.captured("num").toInt();
                //we know it is whites move if the move number just changed
                //TODO: figure out how to determine for sure if it is white or black's move
                //ccm.whitesMove = (lastMoveNum != ccm.num);
                lastMoveNum = ccm.num;
                ccm.secondsLeft = ma2.captured("timeleft").toLong();
                move_list.append(ccm);
            }
        }else{
            qDebug() << "curlybrace_moves empty!";
        }


        //TODO: send played moves so client can resume an adjourned game
        emit onMoveList(match.captured(2).toLong(), match.captured(3), move_list);

        //DG_MOVE_LIST(25 464 * {g3 g2g3 1 59}{d5 d7d5 1 59}{Bg2 f1g2 0 59}{Nf6 g8f6 1 58}{Nf3 g1f3 0 58}{g6 g7g6 0 58}{O-O e1g1c 0 58}{Bg7 f8g7 0 58}{b3 b2b3 0 58}{c6 c7c6 0 58}{Bb2 c1b2 0 57}{O-O e8g8c 1 57}{d3 d2d3 0 57}{Qa5 d8a5 1 56}{Nbd2 b1d2 1 56}{Bg4 c8g4 1 55}{a3 a2a3 1 55}{Nbd7 b8d7 3 52}{b4 b3b4 1 54}{Qc7 a5c7 1 51}{c4 c2c4 0 54}{e6 e7e6 2 49}{Rc1 a1c1 1 53}{Rfe8 f8e8 1 48}{h3 h2h3 1 52}{Bxf3 g4f3n 2 46}{Nxf3 d2f3b 0 52}{Rad8 a8d8 2 44}{Qc2 d1c2 1 51}{Qb8 c7b8 1 43}{Rfd1 f1d1 1 50}{e5 e6e5 1 42}{Qb1 c2b1 1 49}{e4 e5e4 2 41}{Nd4 f3d4 1 49}{e3 e4e3 2 39}{f4 f2f4 3 46}{Nh5 f6h5 0 39}{Kh2 g1h2 1 45}{dxc4 d5c4p 1 38}{dxc4 d3c4p 2 43}{Ndf6 d7f6 2 36}{Nc2 d4c2 4 39}{Nxg3 h5g3p 1 35}{Kxg3 h2g3n 1 38}{Nh5+ f6h5 1 33}{Kh2 g3h2 2 36}{Qxf4+ b8f4p 0 33}{Kg1 h2g1 1 36}{Qf2+ f4f2 1 32}{Kh1 g1h1 1 35}{Ng3+ h5g3 3 28}{Kh2 h1h2 1 34}{Nxe2 g3e2p 0 28}{Bxg7 b2g7b 7 27}{Qf4+ f2f4 3 25}{Kh1 h2h1 2 25}{Kxg7 g8g7b 0 25}{Rf1 d1f1 4 21}{Ng3+ e2g3 2 23}{Kg1 h1g1 0 21}{Nxf1 g3f1r 1 22}{Rxf1 c1f1n 0 21}{Qe5 f4e5 4 18}{Qe1 b1e1 1 20}{e2 e3e2 2 16}{Rf2 f1f2 2 17}{Rd1 d8d1 1 16}{Bf3 g2f3 1 16}{Rxe1+ d1e1q 1 14}{Nxe1 c2e1r 0 16}{Qg3+ e5g3 0 14}{Bg2 f3g2 1 15}{f5 f7f5 2 12}{Kh1 g1h1 1 15}{f4 f5f4 0 12}{Kg1 h1g1 1 14}{f3 f4f3 1 11}{Nxf3 e1f3p 2 11}{e1=Q+ e2e1Q 1 10}{Nxe1 f3e1q 1 11}{Rxe1+ e8e1n 0 10}{Rf1 f2f1 0 10}{Qe3+ g3e3 1 10}{Kh2 g1h2 1 9}{Rxf1 e1f1r 0 10}{Bxf1 g2f1r 0 9}{Qf2+ e3f2 0 9}{Bg2 f1g2 1 8}{g5 g6g5 0 9}{Kh1 h2h1 1 7}{Kf6 g7f6 0 9}{b5 b4b5 1 7})

    }else if (dg == DG_MATCH){
        QString regex = "\\((\\d+) (?P<challname>[^ ]+) (?P<challrating>\\d+) (?P<challratingtype>\\d) \\{(?P<challtitles>)\\} (?P<receiname>[^ ]+) (?P<receirating>\\d+) (?P<receiratingtype>\\d) \\{(?P<receititles>)\\} (?P<wildnumber>\\d+) (?P<ratingtype>[^ ]+) (?P<israted>\\d) (?P<isadjourned>\\d) (?P<challinitialmin>\\d+) (?P<challincsec>\\d+) (?P<receiverinitialmin>\\d+) (?P<receiverincsec>\\d+) (?P<challcolorrequest>\\-?\\d+)( (?P<assess>[^ ]+ [^ ]+ [^ ]+))? (?P<fancytimecontrol>[^ ]+)\\)";
        QRegularExpression re(regex);
        QRegularExpressionMatch match = re.match(unparsedDg);
        qDebug() << "Is a valid match? " << match.isValid()
                 << "challname: " << match.captured("challname")
                 << "challrating: " << match.captured("challrating")
                 << "challratingtype: " << match.captured("challratingtype")
                 << "challtitles: " << match.captured("challtitles")
                 << "receiname: " << match.captured("receiname")
                 << "receirating: " << match.captured("receirating")
                 << "receiratingtype: " << match.captured("receiratingtype")
                 << "receititles: " << match.captured("receititles")
                 << "wildnumber: " << match.captured("wildnumber")
                 << "ratingtype: " << match.captured("ratingtype")
                 << "israted: " << match.captured("israted")
                 << "isadjourned: " << match.captured("isadjourned")
                 << "challinitialmin: " << match.captured("challinitialmin")
                 << "challincsec: " << match.captured("challincsec")
                 << "receiverinitialmin: " << match.captured("receiverinitialmin")
                 << "receiverincsec: " << match.captured("receiverincsec")
                 << "challcolorrequest: " << match.captured("challcolorrequest")
                 << "assess: " << match.captured("assess");

        IccDgMatch dgMatch;

        dgMatch.challname = match.captured("challname");
        dgMatch.challrating = match.captured("challrating").toInt();
        dgMatch.challratingtype = match.captured("challratingtype").toInt();
        dgMatch.challtitles = match.captured("challtitles");
        dgMatch.receiname = match.captured("receiname");
        dgMatch.receirating = match.captured("receirating").toInt();
        dgMatch.receiratingtype = match.captured("receiratingtype").toInt();
        dgMatch.receititles = match.captured("receititles");
        dgMatch.wildnumber = match.captured("wildnumber").toInt();
        dgMatch.ratingtype = match.captured("ratingtype");
        dgMatch.israted = (bool)match.captured("israted").toInt();
        dgMatch.isadjourned = (bool)match.captured("isadjourned").toInt();
        dgMatch.challinitialmin = match.captured("challinitialmin").toInt();
        dgMatch.challincsec = match.captured("challincsec").toInt();
        dgMatch.receiverinitialmin = match.captured("receiverinitialmin").toInt();
        dgMatch.receiverincsec = match.captured("receiverincsec").toInt();
        dgMatch.challcolorrequest = match.captured("challcolorrequest").toInt();
        dgMatch.assess = match.captured("assess");

        emit onMatch(dgMatch);
        /*
         *(29 TwoEqualsOne 1373 2 {} relipse 1733 1 {} 0 Bullet 0 0 1 0 1 0 -1 {}) Challenge: TwoEqualsOne (1373) relipse (1733) unrated Bullet 1 0 You may accept this with "accept TwoEqualsOne", decline it with "decline TwoEqualsOne", or propose different parameters. aics%
DG_MATCH(29 TwoEqualsOne 1373 2 {} relipse 1733 1 {} 0 Bullet 0 0 1 0 1 0 -1 {})
(11 vicilio E 858)
DG_STATE(11 vicilio E 858)
Creating: TwoEqualsOne (1373) relipse (1733) unrated Bullet 1 0 You accept the challenge of TwoEqualsOne. (30 TwoEqualsOne relipse {You accept the challenge of TwoEqualsOne. }){Game 959 (relipse vs. TwoEqualsOne) Creating unrated bullet match.} * relipse vs TwoEqualsOne White> win loss draw score | win loss draw score
DG_PLAYERS_IN_MY_GAME(20 959 TwoEqualsOne PB 1)
DG_MY_GAME_STARTED(15 959 relipse TwoEqualsOne 0 Bullet 0 1 0 1 0 1 {} 1733 1373 1624455577 {} {} 0 0 0 {} 0)
DG_MY_RELATION_TO_GAME(43 959 PW)
DG_PLAYERS_IN_MY_GAME(20 959 relipse PW 1)
DG_FLIP(39 959 0)
DG_MOVE_LIST(25 959 * )
DG_SET_CLOCK(38 959 60 60)
aics%
aics%
Game 959 (relipse vs. TwoEqualsOne)(24 959 e4 e2e4 23 37) --------------------------------- 8 | *R| *N| *B| *Q| *K| *B| *N| *R| Move # : 1 (Black) |---+---+---+---+---+---+---+---| 7 | *P| *P| *P| *P| *P| *P| *P| *P| White Moves : 'e4 (0:23)' |---+---+---+---+---+---+---+---| 6 | | | | | | | | | |---+---+---+---+---+---+---+---| 5 | | | | | | | | | Black Clock : 1 : 00 |---+---+---+---+---+---+---+---| 4 | | | | | P | | | | White Clock : 0 : 37 |---+---+---+---+---+---+---+---| 3 | | | | | | | | | Black Strength : 39 |---+---+---+---+---+---+---+---| 2 | P | P | P | P | | P | P | P | White Strength : 39 |---+---+---+---+---+---+---+---| 1 | R | N | B | Q | K | B | N | R | --------------------------------- a b c d e f g h aics%
DG_SEND_MOVES(24 959 e4 e2e4 23 37)
Game 959 (relipse vs. TwoEqualsOne)(24 959 c5 c7c5 3 57) --------------------------------- 8 | *R| *N| *B| *Q| *K| *B| *N| *R| Move # : 2 (White) |---+---+---+---+---+---+---+---| 7 | *P| *P| | *P| *P| *P| *P| *P| Black Moves : 'c5 (0:03)' |---+---+---+---+---+---+---+---| 6 | | | | | | | | | |---+---+---+---+---+---+---+---| 5 | | | *P| | | | | | Black Clock : 0 : 57 |---+---+---+---+---+---+---+---| 4 | | | | | P | | | | White Clock : 0 : 37 |---+---+---+---+---+---+---+---| 3 | | | | | | | | | Black Strength : 39 |---+---+---+---+---+---+---+---| 2 | P | P | P | P | | P | P | P | White Strength : 39 |---+---+---+---+---+---+---+---| 1 | R | N | B | Q | K | B | N | R | --------------------------------- a b c d e f g h aics%
DG_SEND_MOVES(24 959 c5 c7c5 3 57)
Game 959 (relipse vs. TwoEqualsOne)(24 959 e5 e4e5 7 31) --------------------------------- 8 | *R| *N| *B| *Q| *K| *B| *N| *R| Move # : 2 (Black) |---+---+---+---+---+---+---+---| 7 | *P| *P| | *P| *P| *P| *P| *P| White Moves : 'e5 (0:07)' |---+---+---+---+---+---+---+---| 6 | | | | | | | | | |---+---+---+---+---+---+---+---| 5 | | | *P| | P | | | | Black Clock : 0 : 57 |---+---+---+---+---+---+---+---| 4 | | | | | | | | | White Clock : 0 : 31 |---+---+---+---+---+---+---+---| 3 | | | | | | | | | Black Strength : 39 |---+---+---+---+---+---+---+---| 2 | P | P | P | P | | P | P | P | White Strength : 39 |---+---+---+---+---+---+---+---| 1 | R | N | B | Q | K | B | N | R | --------------------------------- a b c d e f g h aics%
DG_SEND_MOVES(24 959 e5 e4e5 7 31)
Game 959 (relipse vs. TwoEqualsOne)(24 959 Nc6 b8c6 2 56) --------------------------------- 8 | *R| | *B| *Q| *K| *B| *N| *R| Move # : 3 (White) |---+---+---+---+---+---+---+---| 7 | *P| *P| | *P| *P| *P| *P| *P| Black Moves : 'Nc6 (0:02)' |---+---+---+---+---+---+---+---| 6 | | | *N| | | | | | |---+---+---+---+---+---+---+---| 5 | | | *P| | P | | | | Black Clock : 0 : 56 |---+---+---+---+---+---+---+---| 4 | | | | | | | | | White Clock : 0 : 31 |---+---+---+---+---+---+---+---| 3 | | | | | | | | | Black Strength : 39 |---+---+---+---+---+---+---+---| 2 | P | P | P | P | | P | P | P | White Strength : 39 |---+---+---+---+---+---+---+---| 1 | R | N | B | Q | K | B | N | R | --------------------------------- a b c d e f g h aics%
DG_SEND_MOVES(24 959 Nc6 b8c6 2 56)
{Game 959 (relipse vs. TwoEqualsOne) relipse resigns} 0-1 (16 959 1 Res 0-1 {White resigns} {B20})Game was not rated. No rating adjustment. Entering examine mode for game 959. (20 959 TwoEqualsOne X 1)(43 959 E)(20 959 relipse E 1)aics%
DG_MY_GAME_RESULT(16 959 1 Res 0-1 {White resigns} {B20})
DG_PLAYERS_IN_MY_GAME(20 959 TwoEqualsOne X 1)
DG_MY_RELATION_TO_GAME(43 959 E)
DG_PLAYERS_IN_MY_GAME(20 959 relipse E 1)
(32 TwoEqualsOne {} 1 {hope that helped})aics%
DG_SHOUT(32 TwoEqualsOne {} 1 {hope that helped})
(32 ROBOadmin {* C} 2 {*** GRANDMASTER Bones is playing string!! Type "observe Bones" to watch the rated 15-minute 15 0 game.})aics%
DG_SHOUT(32 ROBOadmin {* C} 2 {*** GRANDMASTER Bones is playing string!! Type "observe Bones" to watch the rated 15-minute 15 0 game.})
(32 relipse {} 0 {yes it did a lot thanks})(shouted to 124 people) aics%
DG_SHOUT(32 relipse {} 0 {yes it did a lot thanks})
Recent games of relipse: Opponent Type ECO End Date 57: - 1733 W 1373 TwoEqualsOne [ Bu 1 0] B20 Res Aug 17 13 04:19 56: - 1541 B 1490 digital [ or 1 0] A00 Mat Aug 17 13 02:10 55: + 1557 B 1434 nearthink [ Mr 3 0] C02 Fla Aug 17 13 00:40 54: + 1546 W 1611 Lzmann [ Mr 3 0] E71 Res Aug 17 13 00:38 53: + 1525 W 1611 KINGZINSKI [ Mr 3 0] D15 Res Aug 17 13 00:33 52: + 1503 B 1678 Coyote-Breat [ Mr 3 0] A01 Fla Aug 17 13 00:27 51: + 1827 W 1600 TrainingBot [20u 20 10] --- Mat Aug 16 13 21:32 50: + 1827 W 1600 TrainingBot [20u 20 10] --- Mat Aug 16 13 21:31 49: + 1827 W 1600 TrainingBot [20u 20 10] --- Mat Aug 16 13 21:30 48: + 1827 W 1600 TrainingBot [20u 20 10] --- Mat Aug 16 13 21:30 47: + 1827 W 1600 TrainingBot [20u 20 10] --- Mat Aug 16 13 21:30 46: + 1827 W 1600 TrainingBot [20u 20 10] --- Mat Aug 16 13 21:21 45: + 1827 W 1600 TrainingBot [20u 20 10] --- Mat Aug 16 13 21:20 44: + 1827 W 1600 TrainingBot [20u 20 10] --- Mat Aug 16 13 21:18 43: + 1827 W 1600 TrainingBot [20u 20 10] --- Mat Aug 16 13 21:17 42: + 1827 W 1600 TrainingBot [20u 20 10] --- Mat Aug 16 13 21:17 41: + 1827 W 1600 TrainingBot [20u 20 10] --- Mat Aug 16 13 21:17 40: + 1827 W 1600 TrainingBot [20u 20 10] --- Mat Aug 16 13 21:16 39: + 1827 W 1600 TrainingBot [20u 20 10] --- Mat Aug 16 13 21:15 38: + 1827 W 1600 TrainingBot [20u 20 10] --- Mat Aug 16 13 21:15 aics%
(32 ROBOadmin {* C} 2 {http://www.chessclub.com/activities/events.html for a list of all the upcoming events on the Internet Chess Club!})aics%
DG_SHOUT(32 ROBOadmin {* C} 2 {http://www.chessclub.com/activities/events.html for a list of all the upcoming events on the Internet Chess Club!})
(32 ROBOadmin {* C} 2 {*** GRANDMASTER YARDBIRD is playing Nikola511!! Type "observe YARDBIRD" to watch the rated 5-minute 5 0 game.})aics%
DG_SHOUT(32 ROBOadmin {* C} 2 {*** GRANDMASTER YARDBIRD is playing Nikola511!! Type "observe YARDBIRD" to watch the rated 5-minute 5 0 game.})
(32 ROBOadmin {* C} 2 {*** GRANDMASTER strongwave is playing Kiketf!! Type "observe strongwave" to watch the rated 3-minute 3 0 game.})aics%
DG_SHOUT(32 ROBOadmin {* C} 2 {*** GRANDMASTER strongwave is playing Kiketf!! Type "observe strongwave" to watch the rated 3-minute 3 0 game.})
(32 ROBOKieb {C TD} 0 {Find out your ratings in the various Wild variants! Type: 'tell ROBOKieb wildfinger'.})aics%
DG_SHOUT(32 ROBOKieb {C TD} 0 {Find out your ratings in the various Wild variants! Type: 'tell ROBOKieb wildfinger'.})
(20 959 TwoEqualsOne O 1)
DG_PLAYERS_IN_MY_GAME(20 959 TwoEqualsOne O 1)
Notification: PTrajkovic has departed. Notification: PTrajkovic has arrived. aics%
(20 959 TwoEqualsOne X 1)
DG_PLAYERS_IN_MY_GAME(20 959 TwoEqualsOne X 1)
(11 vicilio E 1203)*/
    }else if (dg == DG_MY_GAME_RESULT){
        //DG_MY_GAME_RESULT(16 959 1 Res 0-1 {White resigns} {B20})
        QString regex = "\\((?P<dg>\\d+) (?P<gamenum>\\d+) (?P<becomeexamined>\\d+) (?P<gameresultcode>\\w+) (?P<scorestring>([012\\-/\\*]+|aborted)) \\{(?<description>.+?)\\} \\{(?P<eco>(\\w+|\\?))\\}\\)";
        QRegularExpression re(regex);
        QRegularExpressionMatch m = re.match(unparsedDg);
        qDebug() << "failed regex: " << regex;
        emit onMyGameResult(m.captured("gamenum").toLong(), (m.captured("becomeexamined").toInt() == 1), m.captured("gameresultcode"), m.captured("scorestring"), m.captured("description"), m.captured("eco"));
    }else{
        //TODO: finish parsing the rest
        //not specifically parsed emit this so clients can parse it
        emit onUnparsedDatagram(dg, unparsedDg);
        return;
    }
    //if execution goes here, that means it successfully parsed and emitted the proper signal
    emit onParseSuccessDatagram(dg, unparsedDg);
}

bool IccClient::parseDgGameStarted(const QString& unparsedDg, IccDgGameStarted &dgGameStarted)
{
    //relipse(1): what characters are allowed in handles? letters and numbers only?
    //Andrey(H)(1): Up to 15 characters, letters and numbers, starting with a letter, and one hyphen allowed in the middle.  See "help username" for more details.  :)

    //"DG_MY_GAME_STARTED"  received:
    //(15 313 relipse relipse 0 Blitz 0 2 12 2 12 0 {Ex: scratch} 0 0 1624451647 {} {} 0 0 0 ? 0)
    QRegularExpression re("\\((?P<dg>\\d+) (?P<gamenumber>\\d+) (?P<whitename>[^ ]+) (?P<blackname>[^ ]+) (?P<wildnumber>\\d+) (?P<ratingtype>[^ ]+) (?P<rated>\\d) (?P<whiteinitialmin>\\d+) (?P<whiteincsec>\\d+) (?P<blackinitialmin>\\d+) (?P<blackincsec>\\d+) (?P<playedgame>\\d) \\{(?P<examinestring>.*?)\\} (?P<whiterating>\\d+) (?P<blackrating>\\d+) (?P<gameid>\\d+) \\{(?P<whitetitles>[^}]*)\\} \\{(?P<blacktitles>[^}]*)\\} (?P<irregularlegality>\\d) (?P<irregularsemantics>\\d) (?P<usesplunkers>\\d) (?P<fancytimecontrol>[^ ]+) (?P<promotetoking>\\d)\\)"
                          ,  QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatchIterator i  = re.globalMatch(unparsedDg);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        qDebug() << "REGEX MATCH GAME_STARTED:" <<  match;
        dgGameStarted.dg = match.captured("dg").toInt();
        dgGameStarted.gamenumber = match.captured("gamenumber").toLong();
        dgGameStarted.whitename = match.captured("whitename");
        dgGameStarted.blackname = match.captured("blackname");
        dgGameStarted.wild_number = match.captured("wildnumber").toInt();
        dgGameStarted.white_initial = match.captured("whiteinitialmin").toInt();
        dgGameStarted.white_increment = match.captured("whiteincsec").toInt();
        dgGameStarted.black_initial = match.captured("blackinitialmin").toInt();
        dgGameStarted.black_increment = match.captured("blackincsec").toInt();
        dgGameStarted.played_game = (bool)match.captured("playedgame").toInt();
        dgGameStarted.ex_string = match.captured("examinestring");
        dgGameStarted.white_rating = match.captured("whiterating").toInt();
        dgGameStarted.black_rating = match.captured("blackrating").toInt();
        dgGameStarted.game_id = match.captured("gameid").toInt();
        dgGameStarted.white_titles = match.captured("whitetitles");
        dgGameStarted.black_titles = match.captured("blacktitles");
        dgGameStarted.irregular_legality = (bool)match.captured("irregularlegality").toInt();
        dgGameStarted.irregular_semantics = (bool)match.captured("irregularsemantics").toInt();
        dgGameStarted.uses_plunkers = (bool)match.captured("usesplunkers").toInt();
        dgGameStarted.fancy_timecontrol = match.captured("fancytimecontrol");
        dgGameStarted.promote_to_king = (bool)match.captured("promotetoking").toInt();
        return true;
    }
    return false;
}

const QString &IccClient::getInterface() const
{
    return m_interface;
}

void IccClient::setInterface(const QString &iface)
{
    m_interface = iface;
    if (this->connected()){
        send(tr("set interface ")+m_interface);
    }
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

