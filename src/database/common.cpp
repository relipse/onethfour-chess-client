/***************************************************************************
 *   (C) 2005 William Hoggarth <whoggarth@users.sourceforge.net>           * 
 *   (C) 2006-2009 Michal Rudolf <mrudolf@kdewebdev.org>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "common.h"

Piece& operator++(Piece& w)
{
    if (w<BlackPawn)
        return w = Piece(w+1);
    else
        return w = Empty;
}

Piece operator++(Piece& w, int)
{
    Piece alt = w;
    ++w;
    return alt;
}

QString resultString(Result result)
{
	switch (result) {
	case WhiteWin:
		return "1-0";
	case Draw:
		return "1/2-1/2";
	case BlackWin:
		return "0-1";
	case ResultUnknown:
	default:
		return "*";
	}
}


char PieceToChar(Piece p)
{
    switch(p){
        case WhiteKing: return 'K';
        case BlackKing: return 'k';
        case WhitePawn: return 'P';
        case BlackPawn: return 'p';
        case WhiteBishop: return 'B';
        case BlackBishop: return 'b';
        case WhiteKnight: return 'N';
        case BlackKnight: return 'n';
        case WhiteQueen: return 'Q';
        case BlackQueen: return 'q';
        case WhiteRook: return 'R';
        case BlackRook: return 'r';
        default:
        //invalid piece or empty
        return '\0';
    }
}


char PieceTypeToChar(PieceType pt)
{
    switch(pt){
        case King: return 'K';
        case Pawn: return 'P';
        case Bishop: return 'B';
        case Knight: return 'N';
        case Queen: return 'Q';
        case Rook: return 'R';;
        default:
        //invalid piece or empty
        return '\0';
    }
}
