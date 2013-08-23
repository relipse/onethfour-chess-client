/****************************************************************
 * The MIT License (MIT)                                         *
 *                                                                *
 *   Copyright (c) 2013 James A. Kinsman                           *
 *                                                                   ********************
 *   Permission is hereby granted, free of charge, to any person obtaining a copy of    *
 *   this software and associated documentation files (the "Software"), to deal in      *
 *   the Software without restriction, including without limitation the rights to       *
 *   use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of   *
 *   the Software, and to permit persons to whom the Software is furnished to do so,    *
 *   subject to the following conditions:                                               *
 *                                                                                      *
 *   The above copyright notice and this permission notice shall be included in all     *
 *   copies or substantial portions of the Software.                                    *
 *                                                                                      *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR         *
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS   *
 *   FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR     *
 *   COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER     *
 *   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN            *
 *   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.         *
 ****************************************************************************************/
#ifndef ICC_H
#define ICC_H

#include <cstring>

// thank you http://www6.chessclub.com/resources/formats/formats.txt
namespace InternetChessClub
{
    const char DGSPLIT = ''; //javascript version here: '\031';
    enum Titles{ GM, IM, FM, WGM, WIM, WFM, TD, C, U, ADMIN, DM, H};

    const char* GetTitleDescription(const char* title);

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
          //DG_POSITION_BEGIN = 110,
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

    const char* DatagramToString(int dg);

    const char* GetIllegalMoveReason(int reason);
}
//ahh, the glories of acronyms
namespace ICC = InternetChessClub;

#endif //ICC_H
