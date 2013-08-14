#ifndef ICC_H
#define ICC_H

// thank you http://www6.chessclub.com/resources/formats/formats.txt
namespace InternetChessClub
{
    const char DGSPLIT = '\031';
    enum Titles{ GM, IM, FM, WGM, WIM, WFM, TD, C, U, ADMIN, DM, H};

    const char* GetTitleDescription(const char* title)
    {
        if (title == "GM"){
            return "grandmaster";
        }else if (title == "IM"){
            return "international master";
        }else if (title == "FM"){
            return "FIDE master";
        }else if (title == "WGM"){
            return "woman grandmaster";
        }else if (title == "WIM"){
            return "woman international master";
        }else if (title == "WFM"){
            return "women FIDE master";
        }else if (title == "TD"){
            return "tournament director";
        }else if (title == "C"){
            return "computer";
        }else if (title == "U"){
            return "unregistered";
        }else if (title == "DM"){
            return "display master";
        }else if (title == "H"){
            return "helper";
        }else if (title == "*"){
            return "administrator";
        }
        return "";
    }

    enum DatagramType{
          DG_WHO_AM_I = 0,
          DG_PLAYER_ARRIVED = 1,
          DG_PLAYER_LEFT = 2,
          DG_BULLET = 3,
          DG_BLITZ = 4,
          DG_STANDARD = 5,
          DG_WILD = 6,
          DG_BUGHOUSE = 7,
          DG_LOSERS = 88,
          DG_CRAZYHOUSE = 121,
          DG_FIVEMINUTE = 125,
          DG_ONEMINUTE = 126,
          DG_CORRESPONDENCE_RATING = 140,
          DG_FIFTEENMINUTE = 145,
          DG_THREEMINUTE = 149,
          DG_FORTYFIVEMINUTE = 150,
          DG_CHESS = 151,
          DG_TIMESTAMP = 8,
          DG_TITLES = 9,
          DG_OPEN = 10,
          DG_STATE = 11,
          DG_PLAYER_ARRIVED_SIMPLE = 55,
          DG_GAME_STARTED = 12,
          DG_GAME_RESULT = 13,
          DG_EXAMINED_GAME_IS_GONE = 14,
          DG_MY_GAME_STARTED = 15,
          DG_MY_GAME_RESULT = 16,
          DG_MY_GAME_ENDED = 17,
          DG_STARTED_OBSERVING = 18,
          DG_STOP_OBSERVING = 19,
          DG_ISOLATED_BOARD = 40,
          DG_PLAYERS_IN_MY_GAME = 20,
          DG_OFFERS_IN_MY_GAME = 21,
          DG_TAKEBACK = 22,
          DG_BACKWARD = 23,
          DG_MOVE_ALGEBRAIC = 33,
          DG_MOVE_SMITH = 34,
          DG_MOVE_TIME = 35,
          DG_MOVE_CLOCK = 36,
          DG_SEND_MOVES = 24,
          DG_MOVE_LIST = 25,
          DG_BUGHOUSE_HOLDINGS = 37,
          DG_BUGHOUSE_PASS = 57,
          DG_KIBITZ = 26,
          DG_SET_CLOCK = 38,
          DG_FLIP = 39,
          DG_REFRESH = 41,
          DG_ILLEGAL_MOVE = 42,
          DG_MY_RELATION_TO_GAME = 43,
          DG_PARTNERSHIP = 44,
          DG_JBOARD = 49,
          DG_FEN = 70,
          DG_MSEC = 56,
          DG_MORETIME = 61,
          DG_CHANNEL_TELL = 28,
          DG_PEOPLE_IN_MY_CHANNEL = 27,
          DG_CHANNELS_SHARED = 46,
          DG_SEES_SHOUTS = 45,
          DG_MATCH = 29,
          DG_MATCH_REMOVED = 30,
          DG_SEEK = 50,
          DG_SEEK_REMOVED = 51,
          DG_MY_RATING = 52,
          DG_NEW_MY_RATING = 87,
          DG_PERSONAL_TELL = 31,
          DG_PERSONAL_TELL_ECHO = 62,
          DG_SHOUT = 32,
          DG_MY_VARIABLE = 47,
          DG_MY_STRING_VARIABLE = 48,
          DG_SOUND = 53,
          DG_SUGGESTION = 63,
          DG_CIRCLE = 59,
          DG_ARROW = 60,
          DG_NOTIFY_ARRIVED = 64,
          DG_NOTIFY_LEFT = 65,
          DG_NOTIFY_OPEN = 66,
          DG_NOTIFY_STATE = 67,
          DG_MY_NOTIFY_LIST = 68,
          DG_LOGIN_FAILED = 69,
          DG_GAMELIST_BEGIN = 72,
          DG_GAMELIST_ITEM = 73,
          DG_IDLE = 74,
          DG_ACK_PING = 75,
          DG_RATING_TYPE_KEY = 76,
          DG_GAME_MESSAGE = 77,
          DG_UNACCENTED = 78,
          DG_STRINGLIST_BEGIN = 79,
          DG_STRINGLIST_ITEM = 80,
          DG_DUMMY_RESPONSE = 81,
          DG_CHANNEL_QTELL = 82,
          DG_PERSONAL_QTELL = 83,
          DG_SET_BOARD = 84,
          DG_MATCH_ASSESSMENT = 85,
          DG_LOG_PGN = 86,
          DG_UNCIRCLE = 89,
          DG_UNARROW = 90,
          DG_WSUGGEST = 91,
          DG_MESSAGELIST_BEGIN = 94,
          DG_MESSAGELIST_ITEM = 95,
          DG_LIST = 96,
          DG_SJI_AD = 97,
          DG_QRETRACT = 99,
          DG_MY_GAME_CHANGE = 100,
          DG_POSITION_BEGIN = 101,
          DG_TOURNEY = 103,
          DG_REMOVE_TOURNEY = 104,
          DG_DIALOG_START = 105,
          DG_DIALOG_DATA = 106,
          DG_DIALOG_DEFAULT = 107,
          DG_DIALOG_END = 108,
          DG_DIALOG_RELEASE = 109,
          DG_POSITION_BEGIN = 110,
          DG_PAST_MOVE = 111,
          DG_PGN_TAG = 112,
          DG_IS_VARIATION = 113,
          DG_PASSWORD = 114,
          DG_WILD_KEY = 116,
          DG_SWITCH_SERVERS = 120,
          DG_SET = 124,
          DG_MUGSHOT = 128,
          DG_TRANSLATION_OKAY = 129,
          DG_UID = 131,
          DG_KNOWS_FISCHER_RANDOM = 132,
          DG_COMMAND = 136,
          DG_TOURNEY_GAME_STARTED = 137,
          DG_TOURNEY_GAME_ENDED = 138,
          DG_MY_TURN = 139,
          DG_DISABLE_PREMOVE = 141,
          DG_PSTAT = 142,
          DG_BOARDINFO = 143,
          DG_MOVE_LAG = 144 };

    const char* DatagramToString(int dg)
    {
        switch(dg)
        {
            case DG_WHO_AM_I: return "DG_WHO_AM_I";
            case DG_PLAYER_ARRIVED: return "DG_PLAYER_ARRIVED";
            case DG_PLAYER_LEFT: return "DG_PLAYER_LEFT";
            case DG_BULLET: return "DG_BULLET";
            case DG_BLITZ: return "DG_BLITZ";
            case DG_STANDARD: return "DG_STANDARD";
            case DG_WILD: return "DG_WILD";
            case DG_BUGHOUSE: return "DG_BUGHOUSE";
            case DG_LOSERS: return "DG_LOSERS";
            case DG_CRAZYHOUSE: return "DG_CRAZYHOUSE";
            case DG_FIVEMINUTE: return "DG_FIVEMINUTE";
            case DG_ONEMINUTE: return "DG_ONEMINUTE";
            case DG_CORRESPONDENCE_RATING: return "DG_CORRESPONDENCE_RATING";
            case DG_FIFTEENMINUTE: return "DG_FIFTEENMINUTE";
            case DG_THREEMINUTE: return "DG_THREEMINUTE";
            case DG_FORTYFIVEMINUTE: return "DG_FORTYFIVEMINUTE";
            case DG_CHESS: return "DG_CHESS";
            case DG_TIMESTAMP: return "DG_TIMESTAMP";
            case DG_TITLES: return "DG_TITLES";
            case DG_OPEN: return "DG_OPEN";
            case DG_STATE: return "DG_STATE";
            case DG_PLAYER_ARRIVED_SIMPLE: return "DG_PLAYER_ARRIVED_SIMPLE";
            case DG_GAME_STARTED: return "DG_GAME_STARTED";
            case DG_GAME_RESULT: return "DG_GAME_RESULT";
            case DG_EXAMINED_GAME_IS_GONE: return "DG_EXAMINED_GAME_IS_GONE";
            case DG_MY_GAME_STARTED: return "DG_MY_GAME_STARTED";
            case DG_MY_GAME_RESULT: return "DG_MY_GAME_RESULT";
            case DG_MY_GAME_ENDED: return "DG_MY_GAME_ENDED";
            case DG_STARTED_OBSERVING: return "DG_STARTED_OBSERVING";
            case DG_STOP_OBSERVING: return "DG_STOP_OBSERVING";
            case DG_ISOLATED_BOARD: return "DG_ISOLATED_BOARD";
            case DG_PLAYERS_IN_MY_GAME: return "DG_PLAYERS_IN_MY_GAME";
            case DG_OFFERS_IN_MY_GAME: return "DG_OFFERS_IN_MY_GAME";
            case DG_TAKEBACK: return "DG_TAKEBACK";
            case DG_BACKWARD: return "DG_BACKWARD";
            case DG_MOVE_ALGEBRAIC: return "DG_MOVE_ALGEBRAIC";
            case DG_MOVE_SMITH: return "DG_MOVE_SMITH";
            case DG_MOVE_TIME: return "DG_MOVE_TIME";
            case DG_MOVE_CLOCK: return "DG_MOVE_CLOCK";
            case DG_SEND_MOVES: return "DG_SEND_MOVES";
            case DG_MOVE_LIST: return "DG_MOVE_LIST";
            case DG_BUGHOUSE_HOLDINGS: return "DG_BUGHOUSE_HOLDINGS";
            case DG_BUGHOUSE_PASS: return "DG_BUGHOUSE_PASS";
            case DG_KIBITZ: return "DG_KIBITZ";
            case DG_SET_CLOCK: return "DG_SET_CLOCK";
            case DG_FLIP: return "DG_FLIP";
            case DG_REFRESH: return "DG_REFRESH";
            case DG_ILLEGAL_MOVE: return "DG_ILLEGAL_MOVE";
            case DG_MY_RELATION_TO_GAME: return "DG_MY_RELATION_TO_GAME";
            case DG_PARTNERSHIP: return "DG_PARTNERSHIP";
            case DG_JBOARD: return "DG_JBOARD";
            case DG_FEN: return "DG_FEN";
            case DG_MSEC: return "DG_MSEC";
            case DG_MORETIME: return "DG_MORETIME";
            case DG_CHANNEL_TELL: return "DG_CHANNEL_TELL";
            case DG_PEOPLE_IN_MY_CHANNEL: return "DG_PEOPLE_IN_MY_CHANNEL";
            case DG_CHANNELS_SHARED: return "DG_CHANNELS_SHARED";
            case DG_SEES_SHOUTS: return "DG_SEES_SHOUTS";
            case DG_MATCH: return "DG_MATCH";
            case DG_MATCH_REMOVED: return "DG_MATCH_REMOVED";
            case DG_SEEK: return "DG_SEEK";
            case DG_SEEK_REMOVED: return "DG_SEEK_REMOVED";
            case DG_MY_RATING: return "DG_MY_RATING";
            case DG_NEW_MY_RATING: return "DG_NEW_MY_RATING";
            case DG_PERSONAL_TELL: return "DG_PERSONAL_TELL";
            case DG_PERSONAL_TELL_ECHO: return "DG_PERSONAL_TELL_ECHO";
            case DG_SHOUT: return "DG_SHOUT";
            case DG_MY_VARIABLE: return "DG_MY_VARIABLE";
            case DG_MY_STRING_VARIABLE: return "DG_MY_STRING_VARIABLE";
            case DG_SOUND: return "DG_SOUND";
            case DG_SUGGESTION: return "DG_SUGGESTION";
            case DG_CIRCLE: return "DG_CIRCLE";
            case DG_ARROW: return "DG_ARROW";
            case DG_NOTIFY_ARRIVED: return "DG_NOTIFY_ARRIVED";
            case DG_NOTIFY_LEFT: return "DG_NOTIFY_LEFT";
            case DG_NOTIFY_OPEN: return "DG_NOTIFY_OPEN";
            case DG_NOTIFY_STATE: return "DG_NOTIFY_STATE";
            case DG_MY_NOTIFY_LIST: return "DG_MY_NOTIFY_LIST";
            case DG_LOGIN_FAILED: return "DG_LOGIN_FAILED";
            case DG_GAMELIST_BEGIN: return "DG_GAMELIST_BEGIN";
            case DG_GAMELIST_ITEM: return "DG_GAMELIST_ITEM";
            case DG_IDLE: return "DG_IDLE";
            case DG_ACK_PING: return "DG_ACK_PING";
            case DG_RATING_TYPE_KEY: return "DG_RATING_TYPE_KEY";
            case DG_GAME_MESSAGE: return "DG_GAME_MESSAGE";
            case DG_UNACCENTED: return "DG_UNACCENTED";
            case DG_STRINGLIST_BEGIN: return "DG_STRINGLIST_BEGIN";
            case DG_STRINGLIST_ITEM: return "DG_STRINGLIST_ITEM";
            case DG_DUMMY_RESPONSE: return "DG_DUMMY_RESPONSE";
            case DG_CHANNEL_QTELL: return "DG_CHANNEL_QTELL";
            case DG_PERSONAL_QTELL: return "DG_PERSONAL_QTELL";
            case DG_SET_BOARD: return "DG_SET_BOARD";
            case DG_MATCH_ASSESSMENT: return "DG_MATCH_ASSESSMENT";
            case DG_LOG_PGN: return "DG_LOG_PGN";
            case DG_UNCIRCLE: return "DG_UNCIRCLE";
            case DG_UNARROW: return "DG_UNARROW";
            case DG_WSUGGEST: return "DG_WSUGGEST";
            case DG_MESSAGELIST_BEGIN: return "DG_MESSAGELIST_BEGIN";
            case DG_MESSAGELIST_ITEM: return "DG_MESSAGELIST_ITEM";
            case DG_LIST: return "DG_LIST";
            case DG_SJI_AD: return "DG_SJI_AD";
            case DG_QRETRACT: return "DG_QRETRACT";
            case DG_MY_GAME_CHANGE: return "DG_MY_GAME_CHANGE";
            case DG_POSITION_BEGIN: return "DG_POSITION_BEGIN";
            case DG_TOURNEY: return "DG_TOURNEY";
            case DG_REMOVE_TOURNEY: return "DG_REMOVE_TOURNEY";
            case DG_DIALOG_START: return "DG_DIALOG_START";
            case DG_DIALOG_DATA: return "DG_DIALOG_DATA";
            case DG_DIALOG_DEFAULT: return "DG_DIALOG_DEFAULT";
            case DG_DIALOG_END: return "DG_DIALOG_END";
            case DG_DIALOG_RELEASE: return "DG_DIALOG_RELEASE";
            case DG_POSITION_BEGIN: return "DG_POSITION_BEGIN";
            case DG_PAST_MOVE: return "DG_PAST_MOVE";
            case DG_PGN_TAG: return "DG_PGN_TAG";
            case DG_IS_VARIATION: return "DG_IS_VARIATION";
            case DG_PASSWORD: return "DG_PASSWORD";
            case DG_WILD_KEY: return "DG_WILD_KEY";
            case DG_SWITCH_SERVERS: return "DG_SWITCH_SERVERS";
            case DG_SET: return "DG_SET";
            case DG_MUGSHOT: return "DG_MUGSHOT";
            case DG_TRANSLATION_OKAY: return "DG_TRANSLATION_OKAY";
            case DG_UID: return "DG_UID";
            case DG_KNOWS_FISCHER_RANDOM: return "DG_KNOWS_FISCHER_RANDOM";
            case DG_COMMAND: return "DG_COMMAND";
            case DG_TOURNEY_GAME_STARTED: return "DG_TOURNEY_GAME_STARTED";
            case DG_TOURNEY_GAME_ENDED: return "DG_TOURNEY_GAME_ENDED";
            case DG_MY_TURN: return "DG_MY_TURN";
            case DG_DISABLE_PREMOVE: return "DG_DISABLE_PREMOVE";
            case DG_PSTAT: return "DG_PSTAT";
            case DG_BOARDINFO: return "DG_BOARDINFO";
            case DG_MOVE_LAG: return "DG_MOVE_LAG";
            default: return "";
        }
    }
}
//ahh, the glories of acronyms
namespace ICC = InternetChessClub;

#endif //ICC_H
