/*
move_sort.h - Copyright (c) 2020 IcebergFu's Software, www.icebergfu.com

search.js - Source Code for XiangQi Wizard Light, Part II

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



#ifndef INCLUDED_MOVE_SORT_H
#define INCLUDED_MOVE_SORT_H

#include <assert.h>
#include "global.h"
#include "position.h"
#include <vector>

void shellSort(std::vector<int> &mvs, std::vector<int>&vls);

class MoveSort {
public:
  MoveSort(int m, Position *p, EntryK *k, int *h) {
    std::vector<int> mvsAll;
    mvs.clear();
    vls.clear();
    mvHash = 0;
    mvKiller1 = 0;
    mvKiller2 = 0;
    pos = p;
    historyTable = h;
    phase = PHASE_HASH;
    index = 0;
    singleReply = false;
    if (pos->inCheck()) {
      phase = PHASE_REST;
      pos->generateMoves(mvsAll);
      for (int i = 0; i < mvsAll.size(); i++) {
        int mv = mvsAll[i];
        if (!pos->makeMove(mv)) {
          continue;
        }
        pos->undoMakeMove();
        mvs.push_back(mv);
        vls.push_back(mv == mvHash ? 0x7fffffff : historyTable[pos->historyIndex(mv)]);
      }
      shellSort(mvs, vls);
      singleReply = (mvs.size() == 1);
    } else {
      mvHash = m;
      mvKiller1 = k[pos->distance].i;
      mvKiller2 = k[pos->distance].j;
    }
  }

  ~MoveSort() {}

  int next();

  std::vector<int> mvs;
  std::vector<int> vls;
  int mvHash;
  int mvKiller1;
  int mvKiller2;
  Position *pos;
  int *historyTable;
  int phase;
  int index;
  bool singleReply;
};

#endif
