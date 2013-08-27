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
        qDebug() << match;
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
        IccDgGameStarted dgGameStarted;
        if (parseDgGameStarted(unparsedDg, dgGameStarted)){
            emit onGameStarted(dgGameStarted);
        }else{
            qDebug() << "error parsing";
        }
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
        QString regex_move_list = "\\((\\d+) (\\d+) (.+?) ?(?<playedmoves>\\{(.*?)\\})*?\\)";
        QRegularExpression re(regex_move_list);
        QRegularExpressionMatch match = re.match(unparsedDg);

        //TODO: send played moves so client can resume an adjourned game
        emit onMoveList(match.captured(2).toLong(), match.captured(3));
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
        QString regex = "\\((?P<dg>\\d+) (?P<gamenum>\\d+) (?P<becomeexamined>\\d+) (?P<gameresultcode>\\w+) (?P<scorestring>([012\\-/\\*]+|aborted)) \\{(?<description>.+?)\\} \\{(?P<eco>(\w+|\\?))\\}\\)";
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

