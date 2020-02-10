/*
board.h - Copyright (c) 2020 IcebergFu's Software, www.icebergfu.com

board.js - Source Code for XiangQi Wizard Light, Part IV

XiangQi Wizard Light - a Chinese Chess Program for JavaScript
Designed by Morning Yellow, Version: 1.0, Last Modified: Sep. 2012
Copyright (C) 2004-2012 www.xqbase.com

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef INCLUDED_BOARD_H
#define INCLUDED_BOARD_H

#include "position.h"
#include "search.h"

extern int IN_BOARD_[256];
//extern char X_PIECE_NAME[24][4];

#define RESULT_UNKNOWN 0
#define RESULT_WIN 1
#define RESULT_DRAW 2

class Board {
public:
  Board() {
    pos = new Position();
    const char *fen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1";
    pos->fromFen(fen);
    search = NULL;
    millis0 = 0;
    millis1 = 0;
    //computer = 0;
    result = RESULT_UNKNOWN;
  }

  ~Board() {
    delete pos;
    delete search;
  }

  //void restart(char *fen);

  void setSearch(int hashLevel) {
    search = new Search(pos, 16);
  }

  //void flushBoard();

  bool IN_BOARD(int sq) {
    return IN_BOARD_[sq] != 0;
  }

  //void drawSquare(int sq);

#if 0
  bool convert(char *myinput) {
    int fx = myinput[0] - '0';
    int fy = myinput[1] - '0';
    int tx = myinput[2] - '0';
    int ty = myinput[3] - '0';

    //printf("%d %d %d %d\n", fx, fy, tx, ty);

    fy = 9 - fy;
    int from_sq = FILE_LEFT + fx + ((RANK_TOP + fy) << 4);
    ty = 9 - ty;
    int to_sq = FILE_LEFT + tx + ((RANK_TOP + ty) << 4);
    //printf("%d %s %d %s\n", from_sq, X_PIECE_NAME[pos->squares[from_sq]], to_sq, X_PIECE_NAME[pos->squares[to_sq]]);

    // Invalid inputs
    if (from_sq < 0 || from_sq > 255 || to_sq < 0 || to_sq > 255)
      return false;

    return addMove(MOVE(from_sq, to_sq));
  }
#endif

  bool addMove(int mv, bool computerMove=false) {
    //printf("addMove %d\n", mv);

    if (!pos->legalMove(mv)) {
      return false;
    }

    if (!pos->makeMove(mv)) {
      return false;
    }

    postAddMove(mv, computerMove);

    return true;
  }

  void postMate(bool computerMove) {
    /*
    if (computerMove) {
      printf("COMPUTER WIN\n");
    } else {
      printf("COMPUTER LOSS\n");
    }
    */
    //printf("MATE\n");
    postAddMove2();
  }

  void postAddMove2() {
  }

  void postAddMove(int mv, bool computerMove) {

    int temp = pos->mvList.back();
    int src = pos->SRC(temp);
    int src_x = (src & 0xf) - FILE_LEFT;
    int src_y = 9 - ((src >> 4) - RANK_TOP);
    int dst = pos->DST(temp);
    int dst_x = (dst & 0xf) - FILE_LEFT;
    int dst_y = 9 - ((dst >> 4) - RANK_TOP);
    //printf("%s %d,%d -> %d,%d ", X_PIECE_NAME[pos->squares[dst]], src_x, src_y, dst_x, dst_y);
    if (pos->inCheck()) {
      //printf("Check\n");
    } else if (pos->captured()) {
      //printf("Capture\n");
    } else {
      //printf("Move\n");
    }

    mvLast = mv;
    if (pos->isMate()) {
      // result = computerMove ? RESULT_LOSS : RESULT_WIN;
      result = RESULT_WIN;
#if 0
      int pc = pos->SIDE_TAG(pos->sdPlayer) + PIECE_KING;
      int sqMate = 0;
      for (int sq = 0; sq < 256; sq++) {
        if (pos->squares[sq] == pc) {
          sqMate = sq;
          break;
        }
      }
      //if (true || sqMate == 0) {
#endif
        postMate(computerMove);
      //  return;
      //}
      return;
    }

    int vlRep = pos->repStatus(3);
    if (vlRep > 0) {
      vlRep = pos->repValue(vlRep);
      if (vlRep > -WIN_VALUE && vlRep < WIN_VALUE) {
        //printf("Repeated no change. DRAW\n");
        result = RESULT_DRAW;
      } /* else if (computerMove == (vlRep < 0)) {
        //printf("Repeted moves. COMPUTER WINS\n");
        //result = RESULT_PLAYER0_WIN;
        //printf("Repeted attacks.\n");
        result = RESULT_WIN;
      } */ else {
        //printf("Repeted moves. COMPUTER LOSES\n");
        //result = RESULT_PLAYER0_WIN;
        //printf("Repeted attacks.\n");
        result = RESULT_WIN;
      }
      postAddMove2();
      return;
    }

    if (pos->captured()) {
      bool hasMaterial = false;
      for (int sq = 0; sq < 256; sq++) {
        if (IN_BOARD(sq) && (pos->squares[sq] & 7) > 2) {
          hasMaterial = true;
          break;
        }
      }
      if (!hasMaterial) {
        result = RESULT_DRAW;
        //printf("No attacking pieces. DRAW\n");
        postAddMove2();
        return;
      }
    } else if (pos->pcList.size() > 100) {
      bool captured = false;
      for (int i = 2; i <= 100; i++) {
        if (pos->pcList[pos->pcList.size() - i] > 0) {
          captured = true;
          break;
        }
      }
      if (!captured) {
        result = RESULT_DRAW;
        //printf("No captures in 100 moves. DRAW\n");
        postAddMove2();
        return;
      }
    }

/*
    int temp = pos->mvList.back();
    int src = pos->SRC(temp);
    int src_x = (src & 0xf) - FILE_LEFT;
    int src_y = 9 - ((src >> 4) - RANK_TOP);
    int dst = pos->DST(temp);
    int dst_x = (dst & 0xf) - FILE_LEFT;
    int dst_y = 9 - ((dst >> 4) - RANK_TOP);
    printf("%s %d,%d -> %d,%d ", X_PIECE_NAME[pos->squares[dst]], src_x, src_y, dst_x, dst_y); 
    if (pos->inCheck()) {
      printf("Check\n");
    } else if (pos->captured()) {
      printf("Capture\n");
    } else {
      printf("Move\n");
    }
*/

    postAddMove2();
  }


  int MOVE(int sqSrc, int sqDst) {
    return sqSrc + (sqDst << 8);
  }

/*
  bool computerMove() {
    // Bit position in computer is the player type
    return ((1 << pos->sdPlayer) & computer) != 0;
  }
  */

/*
  bool playerIsComputer(int player) {
    // Bit position in computer is the player type
    return ((1 << player) & computer) != 0;
  }
  */

  Position *pos;
  //int computer;
  int millis0;
  int millis1;
  Search *search;
  int mvLast;
  int result;
};


#endif
