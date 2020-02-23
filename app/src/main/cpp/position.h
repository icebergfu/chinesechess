/*
position.h - Copyright (c) 2020 IcebergFu's Software, www.icebergfu.com

position.js - Source Code for XiangQi Wizard Light, Part I

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


#ifndef INCLUDED_POSITION_H
#define INCLUDED_POSITION_H

#include <stdio.h>
#include <assert.h>
#include <vector>
#include <stdlib.h>
#include "global.h"
#include "book.h"

#include <android/log.h>
#define MYAPPNAME "Position"

#define RANK_TOP 3
#define RANK_BOTTOM 12
#define FILE_LEFT 3
#define FILE_RIGHT 11

#define PIECE_KING 0
#define PIECE_ADVISOR 1
#define PIECE_BISHOP 2
#define PIECE_KNIGHT 3
#define PIECE_ROOK 4
#define PIECE_CANNON 5
#define PIECE_PAWN 6

#define DEL_PIECE true
#define ADD_PIECE false

extern int PreGen_zobristKeyTable[14][256];
extern int PreGen_zobristLockTable[14][256];
extern int PreGen_zobristKeyPlayer;
extern int PreGen_zobristLockPlayer;

extern int PIECE_VALUE[7][256];

extern int IN_BOARD_[256];
extern int IN_FORT_[256]; 
extern int LEGAL_SPAN[512];
extern int KNIGHT_PIN_[512];

extern int KING_DELTA[4];
extern int ADVISOR_DELTA[4];
extern int KNIGHT_CHECK_DELTA[4][2];

extern int MVV_VALUE[8];


class RC4 {
public:
  RC4(std::vector<int> &key) {
    state.clear();
    x = 0;
    y = 0;
    int i;
    for (i = 0; i < 256; i++) {
      state.push_back(i);
    }
    int j = 0;
    for (i = 0; i < 256; i++) {
      j = (j + state[i] + key[i % key.size()]) & 0xff;
      swap(i, j);
      //printf("%d %d %d %d\n", i, j, state[i], state[j]);
    }
  }

  ~RC4() {}

  int nextByte() {
    x = (x + 1) & 0xff;
    y = (y + state[x]) & 0xff;
    swap(x, y);
    int t = (state[x] + state[y]) & 0xff;
    return state[t];
  }

  int nextLong() {
    int n0 = nextByte();
    int n1 = nextByte();
    int n2 = nextByte();
    int n3 = nextByte();
    return n0 + (n1 << 8) + (n2 << 16) + (n3 <<24);
  }

  /*
  void print() {
    for(int i = 0; i < 256; i++) {
      printf("%d %d\n", i, state[i]);
    }
  }
   */

  void swap(int i, int j) {
    int t = state[i];
    state[i] = state[j];
    state[j] = t;
  }

  int x;
  int y;
  std::vector<int> state;
};

void setup_keys();

class Position {
public:
  Position() {
  }

  ~Position() {}

  void clearBoard() {
    sdPlayer = 0;
    for (int i = 0; i < 256; i++) {
      squares[i] = 0;
    }
    zobristKey = 0;
    zobristLock = 0;
    vlWhite = 0;
    vlBlack = 0;

    mvList.clear();
    pcList.clear();
    keyList.clear();
    chkList.clear();
    distance = 0;
  }

  void setIrrev() {
    mvList.push_back(0);
    pcList.push_back(0);
    keyList.push_back(0);
    chkList.push_back(CHECKED());
    distance = 0;
  }

  bool CHECKED() {
    int pcSelfSide = SIDE_TAG(sdPlayer);
    int pcOppSide = OPP_SIDE_TAG(sdPlayer);
    //printf("checked %d %d\n", pcSelfSide, pcOppSide);
    int sqSrc;
    int delta;
    int i, j;
    for (sqSrc = 0; sqSrc < 256; sqSrc++) {
      if (squares[sqSrc] != pcSelfSide + PIECE_KING) {
        continue;
      }
      if (squares[SQUARE_FORWARD(sqSrc, sdPlayer)] == pcOppSide + PIECE_PAWN) {
        //printf("pawn0\n");
        return true;
      }
      for (delta = -1; delta <= 1; delta += 2) {
        if (squares[sqSrc + delta] == pcOppSide + PIECE_PAWN) {
          //printf("pawn1\n");
          return true;
        }
      }
      for (i = 0; i < 4; i++) {
        if (squares[sqSrc + ADVISOR_DELTA[i]] != 0) {
          continue;
        }
        for (j = 0; j < 2; j++) {
          int pcDst = squares[sqSrc + KNIGHT_CHECK_DELTA[i][j]];
          if (pcDst == pcOppSide + PIECE_KNIGHT) {
            //printf("knight\n");
            return true;
          }
        }
      }

      for (i = 0; i < 4; i++) {
        delta = KING_DELTA[i];
        int sqDst = sqSrc + delta;
        while (IN_BOARD(sqDst)) {
          int pcDst = squares[sqDst];
          if (pcDst > 0) {
            if (pcDst == pcOppSide + PIECE_ROOK || pcDst == pcOppSide + PIECE_KING) {
              //printf("root or king\n");
              return true;
            }
            break;
          }
          sqDst += delta;
        }
        sqDst += delta;
        while (IN_BOARD(sqDst)) {
          int pcDst = squares[sqDst];
          if (pcDst > 0) {
            if (pcDst == pcOppSide + PIECE_CANNON) {
              //printf("cannon\n");
              return true;
            }
            break;
          }
          sqDst += delta;
        }
      }
      return false;
    }

    return false;
  }

  void fromFen(const char *fen);

  int CHAR_TO_PIECE(int c) {
    if (c == 'K') return PIECE_KING;
    if (c == 'A') return PIECE_ADVISOR;
    if (c == 'B' || c == 'E') return PIECE_BISHOP;
    if (c == 'H' || c == 'N') return PIECE_KNIGHT;
    if (c == 'R') return PIECE_ROOK;
    if (c == 'C') return PIECE_CANNON;
    if (c == 'P') return PIECE_PAWN;
    assert(0);
    return -1;
  }

  int COORD_XY(int x, int y) {
    return x + (y << 4);
  }

  int FILE_X(int sq) {
    return sq & 15;
  }

  int RANK_Y(int sq) {
    return sq >> 4;
  }

  int FILE_FLIP(int x) {
    return 14 - x;
  }

  int MIRROR_SQUARE(int sq) {
    return COORD_XY(FILE_FLIP(FILE_X(sq)), RANK_Y(sq));
  }

  int SQUARE_FLIP(int sq) {
    return 254 - sq;
  }

  void addPiece(int sq, int pc, bool bDel=false) {
    //printf("CFU sq %d pc %d del %d\n", sq, pc, (int)bDel);
    int pcAdjust = 0;
    squares[sq] = bDel ? 0 : pc;
    if (pc < 16) {
      pcAdjust = pc - 8;
      vlWhite += bDel ? -PIECE_VALUE[pcAdjust][sq] : PIECE_VALUE[pcAdjust][sq];
    } else {
      pcAdjust = pc - 16;
      vlBlack += bDel ? -PIECE_VALUE[pcAdjust][SQUARE_FLIP(sq)] : PIECE_VALUE[pcAdjust][SQUARE_FLIP(sq)];
      pcAdjust += 7;
    }
    zobristKey ^= PreGen_zobristKeyTable[pcAdjust][sq]; 
    zobristLock ^= PreGen_zobristLockTable[pcAdjust][sq]; 
    //printf("zobristLock = %x\n", zobristLock);
  }

  void changeSide() {
    sdPlayer = 1 - sdPlayer;
    zobristKey ^= PreGen_zobristKeyPlayer;
    zobristLock ^= PreGen_zobristLockPlayer;
    // printf("change side to %d zobristLock %x\n", sdPlayer, zobristLock);
  }

  int SRC(int mv) {
    return mv & 255;
  }

  int DST(int mv) {
    return mv >> 8;
  }

  int SIDE_TAG(int sd) {
    // player0 = red is 8
    // player1 = black is 16
    return 8 + (sd << 3);
  }

  int OPP_SIDE_TAG(int sd) {
    // red is 8
    // black is 16
    return 16 - (sd << 3);
  }

  bool legalMove(int mv) {
    int sqSrc = SRC(mv);
    int pcSrc = squares[sqSrc];
    int pcSelfSide = SIDE_TAG(sdPlayer);
    //printf("sqSrc %d, pcSrc %d, pcSelfSide %d\n", sqSrc, pcSrc, pcSelfSide);
    if ((pcSrc & pcSelfSide) == 0) {
      //printf("Wrong src side\n");
      return false;
    }

    int sqDst = DST(mv);
    int pcDst = squares[sqDst];
    // printf("sqDst %d, pcDst %d\n", sqDst, pcDst);
    if ((pcDst & pcSelfSide) != 0) {
      //printf("Wrong dst side\n");
      return false;
    }

    int sqPin;
    int delta;
    int this_piece = pcSrc - pcSelfSide;
    switch (this_piece) {
    case PIECE_KING:
      return IN_FORT(sqDst) && KING_SPAN(sqSrc, sqDst);

    case PIECE_ADVISOR:
      return IN_FORT(sqDst) && ADVISOR_SPAN(sqSrc, sqDst);

    case PIECE_BISHOP:
      return SAME_HALF(sqSrc, sqDst) && BISHOP_SPAN(sqSrc, sqDst) &&
        squares[BISHOP_PIN(sqSrc, sqDst)] == 0;

    case PIECE_KNIGHT:
      sqPin = KNIGHT_PIN(sqSrc, sqDst);
      return sqPin != sqSrc && squares[sqPin] == 0;

    case PIECE_ROOK:
    case PIECE_CANNON:
      if (SAME_RANK(sqSrc, sqDst)) {
        delta = (sqDst < sqSrc) ? -1 : 1;
      } else if (SAME_FILE(sqSrc, sqDst)) {
        delta = (sqDst < sqSrc) ? -16 : 16;
      } else {
        return false;
      }
      sqPin = sqSrc + delta;
      for (; sqPin != sqDst && squares[sqPin] == 0; sqPin += delta)
        ;
      
      if (sqPin == sqDst) {
        return pcDst == 0 || this_piece == PIECE_ROOK;
      }
      if (pcDst == 0 || this_piece == PIECE_ROOK) {
        return false;
      }
      sqPin += delta;
      for (; sqPin != sqDst && squares[sqPin] == 0; sqPin += delta)
        ;
      return sqPin == sqDst;

    case PIECE_PAWN:
      if (AWAY_HALF(sqDst, sdPlayer) && (sqDst == sqSrc - 1 || sqDst == sqSrc + 1)) {
        return true;
      }
      return sqDst == SQUARE_FORWARD(sqSrc, sdPlayer);

    default:
      assert(0);
      return false;
    }

    assert(0);
    return false;
  }

  int BISHOP_PIN(int sqSrc, int sqDst) {
    return (sqSrc + sqDst) >> 1;
  }

  int KNIGHT_PIN(int sqSrc, int sqDst) {
    return sqSrc + KNIGHT_PIN_[sqDst - sqSrc + 256];
  }

  bool IN_FORT(int sq) {
    return IN_FORT_[sq] != 0;
  }

  bool KING_SPAN(int sqSrc, int sqDst) {
    return LEGAL_SPAN[sqDst - sqSrc + 256] == 1;
  }

  bool ADVISOR_SPAN(int sqSrc, int sqDst) {
    return LEGAL_SPAN[sqDst - sqSrc + 256] == 2;
  }

  bool BISHOP_SPAN(int sqSrc, int sqDst) {
    return LEGAL_SPAN[sqDst - sqSrc + 256] == 3;
  }

  bool SAME_HALF(int sqSrc, int sqDst) {
    return ((sqSrc ^ sqDst) & 0x80) == 0;
  }

  bool SAME_RANK(int sqSrc, int sqDst) {
    return ((sqSrc ^ sqDst) & 0xf0) == 0;
  }

  bool SAME_FILE(int sqSrc, int sqDst) {
    return ((sqSrc ^ sqDst) & 0x0f) == 0;
  }


  void movePiece(int mv) {
    int sqSrc = SRC(mv);
    int sqDst = DST(mv);
    int pc = squares[sqDst];
    pcList.push_back(pc);
    if (pc > 0) {
      addPiece(sqDst, pc, DEL_PIECE);
    }
    pc = squares[sqSrc];
    addPiece(sqSrc, pc, DEL_PIECE);
    addPiece(sqDst, pc, ADD_PIECE);
    mvList.push_back(mv);
  }

  void undoMovePiece() {
    int mv = mvList.back();
    mvList.pop_back();
    int sqSrc = SRC(mv);
    int sqDst = DST(mv);
    int pc = squares[sqDst];
    addPiece(sqDst, pc, DEL_PIECE);
    addPiece(sqSrc, pc, ADD_PIECE);
    pc = pcList.back();
    pcList.pop_back();
    if (pc > 0) {
      addPiece(sqDst, pc, ADD_PIECE);
    }
  }

  void undoMakeMove() {
    distance--;
    chkList.pop_back();
    changeSide();
    keyList.pop_back();
    undoMovePiece();
  }

  bool makeMove(int mv) {
    // printf("CFU makeMove\n");
    int zkey = zobristKey;
    movePiece(mv);
    if (CHECKED()) {
      //assert(0);
      undoMovePiece();
      return false;
    }
    keyList.push_back(zkey);
    changeSide();
    chkList.push_back(CHECKED());
    distance++;
    return true;
  }

  void nullMove() {
    mvList.push_back(0);
    pcList.push_back(0);
    keyList.push_back(zobristKey);
    changeSide();
    chkList.push_back(false);
    distance++;
  }

  void undoNullMove() {
    distance--;
    chkList.pop_back();
    changeSide();
    keyList.pop_back();
    pcList.pop_back();
    mvList.pop_back();
  }

  bool HOME_HALF(int sq, int sd) {
    return (sq & 0x80) != (sd << 7);
  }

  bool AWAY_HALF(int sq, int sd) {
    return (sq & 0x80) == (sd << 7);
  }

  int SQUARE_FORWARD(int sq, int sd) {
    return sq - 16 + (sd << 5);
  }

  bool IN_BOARD(int sq) {
    return IN_BOARD_[sq] != 0;
  }

  int binarySearch(int vl) {
    //printf("binarySearch %d\n", vl);
    int low = 0;
    int high = BOOK_DAT_LENGTH - 1;
    while (low <= high) {
      int mid = (low + high) >> 1;
      if (BOOK_DAT[mid][0] < vl) {
        low = mid + 1;
      } else if (BOOK_DAT[mid][0] > vl) {
        high = mid - 1;
      } else {
        return mid;
      }
    }
    return -1;
  }

  void mirror_this(Position &mirror_pos) {
    mirror_pos.clearBoard();
    for (int sq = 0; sq < 256; sq++) {
      int pc = squares[sq];
      if (pc > 0) {
        mirror_pos.addPiece(MIRROR_SQUARE(sq), pc);
      }
    }
    if (sdPlayer == 1) {
      mirror_pos.changeSide();
    }
  }

  int bookMove() {
    bool mirror = false;
    //printf("CFU bookMove %x\n", zobristLock);
    int lock;

    if (sdPlayer == 1) {
      lock = zobristLock ^ PreGen_zobristLockPlayer;
    }
    else {
      lock = zobristLock;
    }
    
    lock = ((unsigned int)lock) >> 1;
    //printf("CFU lock %x\n", lock);
    int index = binarySearch(lock);
    //printf("find %d\n", index);
    if (index < 0) {
      mirror = true;
      Position mirror_pos; 
      mirror_this(mirror_pos);
      //printf("CFU bookMove mirror %x\n", mirror_pos.zobristLock);
      lock = ((unsigned int)mirror_pos.zobristLock) >> 1;
      index = binarySearch(lock);
      //printf("mirror find %d\n", index);
    }
    if (index < 0) {
      return 0;
    }

    index--;
    while (index >= 0 && BOOK_DAT[index][0] == lock) {
      index--;
    }
    index++;
    //printf("final find %d %d %d\n", index, BOOK_DAT[index-1][0], BOOK_DAT[index][0]);
    std::vector<int> mvs, vls;
    int value = 0;

    while (index < BOOK_DAT_LENGTH && BOOK_DAT[index][0] == lock) {
      // printf("Test %d\n", index);
      int mv = BOOK_DAT[index][1];
      mv = (mirror ? MIRROR_MOVE(mv) : mv);
      if (legalMove(mv)) {
        mvs.push_back(mv);
        int vl = BOOK_DAT[index][2];
        vls.push_back(vl);
        value += vl;
      }

      index++;
    } 

    if (value == 0) {
      return 0;
    }

    int new_value = 1 + ((unsigned int)rand()) % value;
    // printf("value = %d new_value = %d\n", value, new_value);
    //__android_log_print(ANDROID_LOG_VERBOSE, MYAPPNAME, "value = %d new_value = %d\n", value, new_value);
    int i; 
    for (i = 0; i < mvs.size(); i++) {
      //printf("%d %d %d\n", i, mvs[i], vls[i]);
      new_value -= vls[i];
      if (new_value <= 0) {
        break;
      }
    }
    //printf("i=%d %d\n", i, mvs[i]);
    //__android_log_print(ANDROID_LOG_VERBOSE, MYAPPNAME, "bookMove i=%d %d\n", i, mvs[i]);
    return mvs[i];
  }


  bool inCheck() {
    return chkList.back();
  }


  int MIRROR_MOVE(int mv) {
    return MOVE(MIRROR_SQUARE(SRC(mv)), MIRROR_SQUARE(DST(mv)));
  }


  int MOVE(int sqSrc, int sqDst) {
    return sqSrc + (sqDst << 8);
  }


  int MVV_LVA(int pc, int lva) {
    return MVV_VALUE[pc & 7] - lva;
  }

  void generateMoves(std::vector<int> &mvs);
  void generateMoves(std::vector<int> &mvs, std::vector<int> &vls);

  int historyIndex(int mv) {
    return ((squares[SRC(mv)] - 8) << 8) + DST(mv);
  }

  int mateValue() {
    return distance - MATE_VALUE;
  }

  int repStatus(int recur_) {
    int recur = recur_;
    int selfSide = false;
    bool perpCheck = true;
    bool oppPerpCheck = true;
    int index = mvList.size() - 1;
    while (mvList[index] > 0 && pcList[index] == 0) {
      // printf("CFU index %d mvList %d pcList %d selfSide %d chkList %d\n", index, mvList[index], pcList[index], selfSide, chkList[index]);
    
      if (selfSide) {
        perpCheck = perpCheck && chkList[index];
        if (keyList[index] == zobristKey) {
          recur--;
          if (recur == 0) {
            return 1 + (perpCheck ? 2 : 0) + (oppPerpCheck ? 4 : 0);
          }
        }
      } else {
        oppPerpCheck = oppPerpCheck && chkList[index]; 
      }
      selfSide = !selfSide;
      index--;
    }
    return 0;
  }

  int repValue(int vlRep) {
    int vlReturn = ((vlRep & 2) == 0 ? 0 : banValue()) +
                   ((vlRep & 4) == 0 ? 0 : -banValue());
    return vlReturn == 0 ? drawValue() : vlReturn; 
  }

  int banValue() {
    return distance - BAN_VALUE;
  }

  int drawValue() {
    return (distance & 1) == 0 ? -DRAW_VALUE : DRAW_VALUE;
  }

  int evaluate() {
    int vl = (sdPlayer == 0 ? vlWhite - vlBlack :
                               vlBlack - vlWhite) + ADVANCED_VALUE;
    return vl == drawValue() ? vl - 1 : vl;
  }

  bool nullOkay() {
    return ((sdPlayer == 0) ? vlWhite : vlBlack) >  NULL_OKAY_MARGIN;
  }

  bool nullSafe() {
    return ((sdPlayer == 0) ? vlWhite : vlBlack) >  NULL_SAFE_MARGIN;
  }

  bool isMate() {
    std::vector<int> mvs;
    generateMoves(mvs);
    for (int i = 0; i < mvs.size(); i++) {
      if (makeMove(mvs[i])) {
        undoMakeMove();
        return false;
      }
    }
    return true;
  }

  bool captured() {
    return pcList[pcList.size() - 1] > 0;
  }

  int sdPlayer;
  int squares[256];
  int zobristKey;
  int zobristLock;
  int vlWhite;
  int vlBlack;

  std::vector<int> mvList;
  std::vector<int> pcList;
  std::vector<int> keyList;
  std::vector<bool> chkList;
  int distance;
};


#endif
