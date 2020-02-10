/*
board.cpp - Copyright (c) 2020 IcebergFu's Software, www.icebergfu.com

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



#include <stdio.h>
#include "board.h"

// empty is 0
// red from 8 .. 14, ?
// back from 16 .. 30, ?
/*
char X_PIECE_NAME[24][4] = {
  "--", "??", "??", "??", "??", "??", "??", "??",
  "帥", "仕", "相", "傌", "俥", "炮", "兵", "??",
  "將", "士", "象", "馬", "車", "砲", "卒", "??",
};
*/
/*
void Board::restart(char *fen) {
  //printf("%s\n", fen);
}
*/

/*
void Board::drawSquare(int sq) {

    if ((sq & 15) == 3)
      printf("%d ", 9 - ((sq >> 4) - 3));
    printf("%s ", X_PIECE_NAME[pos->squares[sq]]);
    if ((sq & 15) == 11)
      printf("\n");

}
*/

#if 0
void Board::flushBoard() {
  mvLast = pos->mvList.back();
  //printf("CFU mvLast = %d\n", mvLast);
  /*
  for (int sq = 0; sq < 256; sq++) {
    if (IN_BOARD(sq)) {
      drawSquare(sq);
    }
  }
   */
  //printf("  0  1  2  3  4  5  6  7  8\n");
}
#endif
