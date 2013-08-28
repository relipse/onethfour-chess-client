#ifndef ICCCLIENT_H
#define ICCCLIENT_H

#include <QObject>
#include <QTcpSocket>
class QTcpSocket;

#include "../icc/icc.h"

#include "chessclient.h"

struct IccDgMatch
{
    QString challname;
    int challrating;
    int challratingtype;
    QString challtitles;
    QString receiname;
    int receirating;
    int receiratingtype;
    QString receititles;
    int wildnumber;
    QString ratingtype;
    bool israted;
    bool isadjourned;
    int challinitialmin;
    int challincsec;
    int receiverinitialmin;
    int receiverincsec;
    int challcolorrequest;
    QString assess;
};


struct IccCurlyChessMove{
    IccCurlyChessMove():algebraic(""),fromTo(""),num(0){}
    QString algebraic;
    QString fromTo;
    int num;
    long secondsLeft;
    bool whitesMove;
};
#include <QMetaType>
Q_DECLARE_METATYPE(QList<IccCurlyChessMove>)
/*
DG_WHO_AM_Irelipse {}
(31 relipse {} {hi} 1)(told relipse) aics%
DG_PERSONAL_TELLrelipse {} {hi} 1
Messages: 1. Canon (05:20 15-Aug-13 EDT): Sorry for the delay in replying to your question. I would suggest you email support@chessclub.com who may be able to assist you with your query. Thank you. aics%
(15 918 relipse relipse 0 Blitz 0 2 12 2 12 0 {Ex: scratch} 0 0 1624436053 {} {} 0 0 0 ? 0)(43 918 E)(20 918 relipse E 1)(39 918 0)(25 918 * ) --------------------------------- 8 | *R| *N| *B| *Q| *K| *B| *N| *R| Move # : 1 (White) |---+---+---+---+---+---+---+---| 7 | *P| *P| *P| *P| *P| *P| *P| *P| |---+---+---+---+---+---+---+---| 6 | | | | | | | | | |---+---+---+---+---+---+---+---| 5 | | | | | | | | | Black Clock : 2 : 00 |---+---+---+---+---+---+---+---| 4 | | | | | | | | | White Clock : 2 : 00 |---+---+---+---+---+---+---+---| 3 | | | | | | | | | Black Strength : 39 |---+---+---+---+---+---+---+---| 2 | P | P | P | P | P | P | P | P | White Strength : 39 |---+---+---+---+---+---+---+---| 1 | R | N | B | Q | K | B | N | R | --------------------------------- a b c d e f g h aics%
DG_MY_GAME_STARTED918 relipse relipse 0 Blitz 0 2 12 2 12 0 {Ex: scratch} 0 0 1624436053 {} {} 0 0 0 ? 0)(43 918 E)(20 918 relipse E 1)(39 918 0)(25 918 *
(24 918 e4 e2e4 0 132)Game 918: relipse moves: e4 Game 918 (relipse vs. relipse) --------------------------------- 8 | *R| *N| *B| *Q| *K| *B| *N| *R| Move # : 1 (Black) |---+---+---+---+---+---+---+---| 7 | *P| *P| *P| *P| *P| *P| *P| *P| White Moves : 'e4 (0:00)' |---+---+---+---+---+---+---+---| 6 | | | | | | | | | |---+---+---+---+---+---+---+---| 5 | | | | | | | | | Black Clock : 2 : 00 |---+---+---+---+---+---+---+---| 4 | | | | | P | | | | White Clock : 2 : 12 |---+---+---+---+---+---+---+---| 3 | | | | | | | | | Black Strength : 39 |---+---+---+---+---+---+---+---| 2 | P | P | P | P | | P | P | P | White Strength : 39 |---+---+---+---+---+---+---+---| 1 | R | N | B | Q | K | B | N | R | --------------------------------- a b c d e f g h aics%
DG_SEND_MOVES918 e4 e2e4 0 132
(24 918 e5 e7e5 3 129)Game 918: relipse moves: e5 Game 918 (relipse vs. relipse) --------------------------------- 8 | *R| *N| *B| *Q| *K| *B| *N| *R| Move # : 2 (White) |---+---+---+---+---+---+---+---| 7 | *P| *P| *P| *P| | *P| *P| *P| Black Moves : 'e5 (0:03)' |---+---+---+---+---+---+---+---| 6 | | | | | | | | | |---+---+---+---+---+---+---+---| 5 | | | | | *P| | | | Black Clock : 2 : 09 |---+---+---+---+---+---+---+---| 4 | | | | | P | | | | White Clock : 2 : 12 |---+---+---+---+---+---+---+---| 3 | | | | | | | | | Black Strength : 39 |---+---+---+---+---+---+---+---| 2 | P | P | P | P | | P | P | P | White Strength : 39 |---+---+---+---+---+---+---+---| 1 | R | N | B | Q | K | B | N | R | --------------------------------- a b c d e f g h aics%
DG_SEND_MOVES918 e5 e7e5 3 129
(42 918 e6 2)Illegal move (e6). aics%
DG_ILLEGAL_MOVE918 e6 2
(gamenumber whitename blackname wild-number rating-type rated
        white-initial white-increment black-initial black-increment
        played-game {ex-string} white-rating black-rating game-id
        white-titles black-titles irregular-legality irregular-semantics
        uses-plunkers fancy-timecontrol promote-to-king)  */


struct IccDgGameStarted
{
    IccDgGameStarted() : dg(0){}
    int dg; //could be 12 or 15 depending on whether it is my game or someone elses
    long int gamenumber;
    QString whitename;
    QString blackname;
    int wild_number;
    QString rating_type;
    bool rated;
    //DG_MY_GAME_STARTED918 relipse relipse 0 Blitz 0
    //2 12 2 12
    //0 {Ex: scratch} 0 0 1624436053 {} {}
    //0 0 0 ? 0)

    int white_initial;
    int white_increment;
    int black_initial;
    int black_increment;

    bool played_game;
    QString ex_string;
    int white_rating;
    int black_rating;
    QString game_id;  //an int is too small, just use QString

    QString white_titles;
    QString black_titles;

    bool irregular_legality;
    bool irregular_semantics;

    bool uses_plunkers;
    QString fancy_timecontrol;
    bool promote_to_king;
};

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
    void onMatch(const IccDgMatch& dgMatch);
    void onMyGameStarted(const IccDgGameStarted& dgMyGameStarted);
    void onGameStarted(const IccDgGameStarted& dgGameStarted);
    void onStartedObserving(const IccDgGameStarted& dgStartedObserving);

    //this tells client whether i am white or black or examining, observing or x game over etc
    void onMyRelationToGame(int game_number, const QString& symbol);
    //whenever moves get sent (from myself or opponent), this signal should update the board officially
    //and the move confirmed
    void onSendMoves(long game_number, const QString& algebraic,const QString& smith, int timetaken, int clock, bool is_variation);
    void onMoveList(long game_number, const QString& position, QList<IccCurlyChessMove>& move_list);
    void onIllegalMove(long game_number, const QString& movestring, int reason);
    void onTakebackMove(long game_number, long takeback_ply);
    void onFlip(long game_number, int flip);
    void onMyGameResult(long game_number, bool becomeexamined, const QString& gameresultcode, const QString& scorestring, const QString& description, const QString& eco);
public:
    void SetUsername(const QString& username);
    void SetPassword(const QString& password);

    const QString&  loggedInHandle();
    const QString&  lastTellFrom();

    void parseDatagram(int dg, const QString& unparsedDg);
    bool parseDgGameStarted(const QString &unparsedDg, IccDgGameStarted &dgGameStarted);
    const QString& getInterface() const;
    void setInterface(const QString &iface);

protected:
    QString m_level2settings;
    QString m_username;
    QString m_password;

    QString m_loggedInHandle;
    QString m_loggedInTitles;

    QString m_lastTellFrom;
    QString m_interface;
};

#endif // ICCCLIENT_H
