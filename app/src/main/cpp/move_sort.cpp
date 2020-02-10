/*
move_sort.cpp - Copyright (c) 2020 IcebergFu's Software, www.icebergfu.com

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


#include "move_sort.h"
#include <iostream>

int SHELL_STEP[] = {0, 1, 4, 13, 40, 121, 364, 1093};

void shellSort(std::vector<int> &mvs, std::vector<int>&vls) 
{
  int i, j;
  int stepLevel = 1;
  while (SHELL_STEP[stepLevel] < mvs.size()) {
    stepLevel++;
  }
  stepLevel--;

  while (stepLevel > 0) {
    int step = SHELL_STEP[stepLevel];
    //printf("step=%d\n", step);
    for (i = step; i < mvs.size(); i++) {
      int mvBest = mvs[i];
      int vlBest = vls[i];
      j = i - step;
      while (j >= 0 && vlBest > vls[j]) {
        mvs[j + step] = mvs[j];
        vls[j + step] = vls[j];
        j -= step;
      }
      mvs[j + step] = mvBest;
      vls[j + step] = vlBest;
    }
    stepLevel--;
  }

  /* 
  for (auto const& x: mvs) {
    std::cout << x << "\n";
  }
  for (auto const& x: vls) {
    std::cout << x << "\n";
  }
  assert(0);
  */
}

int MoveSort::next() {
  //int i;
  switch(phase) {
  case PHASE_HASH:
    phase = PHASE_KILLER_1;
    if (mvHash > 0) {
      return mvHash;
    }
    // fall through

  case PHASE_KILLER_1:
    phase = PHASE_KILLER_2;
    if (mvKiller1 != mvHash && mvKiller1 > 0 && pos->legalMove(mvKiller1))  {
      return mvKiller1;
    }
    // fall through

  case PHASE_KILLER_2:
    phase = PHASE_GEN_MOVES;
    if (mvKiller2 != mvHash && mvKiller2 > 0 && pos->legalMove(mvKiller2))  {
      return mvKiller2;
    }
    // fall through

  case PHASE_GEN_MOVES:
    phase = PHASE_REST;
    pos->generateMoves(mvs);
    vls.clear();

    //printf("len=%d\n", mvs.size());
    for (auto const& x: mvs) {
      // printf("CFU %d %d %d\n", x, pos->historyIndex(x), historyTable[pos->historyIndex(x)]);
      vls.push_back(historyTable[pos->historyIndex(x)]);
    }
    shellSort(mvs, vls);
    index = 0;
    // fall-through

  default:
    while (index < mvs.size()) {
      int mv = mvs[index];
      index++;
      if (mv != mvHash && mv != mvKiller1 && mv != mvKiller2) {
        //printf("CFU %d\n", mv);
        return mv;
      }
    }
  }

  return 0;
}
