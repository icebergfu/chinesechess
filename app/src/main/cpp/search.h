/*
search.h - Copyright (c) 2020 IcebergFu's Software, www.icebergfu.com

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

#ifndef INCLUDED_SEARCH_H
#define INCLUDED_SEARCH_H

#define LIMIT_DEPTH 64

#include <time.h>
#include "position.h"
#include "move_sort.h"
#include "global.h"


class Search {
public:
  Search(Position *p, int hashLevel):pos(p) {
    hashMask = (1 << hashLevel) - 1;
    hashTable = new Entry[1 << hashLevel];
 }

  ~Search() {
    delete[] hashTable;
  }

  int searchMain0();
  int searchMain1();
  int searchMain2(int limit_depth, int millis);

  int probeHash(int vlAlpha, int vlBeta, int depth, std::vector<int> &mv);
  int searchMain(int limit_depth, int millis);
  int searchRoot(int depth);
  int searchFull(int vlAlpha_, int vlBeta, int depth, bool noNull);
  int searchQuiesc(int vlAlpha_, int vlBeta);
  bool searchUnique(int vlBeta, int depht);

  void recordHash(int hashFlag, int vlBest, int depth, int mvBest);
  void setBestMove(int mvBest, int depth);

  Entry *getHashItem() {
    return &hashTable[pos->zobristKey & hashMask];
  }

  int hashMask;
  Position *pos;
  Entry *hashTable;
  EntryK killerTable[LIMIT_DEPTH];
  int historyTable[4096];
  int mvResult;
  int allNodes;


  struct timespec my_start;
  bool STOP_SEARCH;
};

#endif
