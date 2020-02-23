/*
search.cpp - Copyright (c) 2020 IcebergFu's Software, www.icebergfu.com

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

#include "search.h"

#include <stdio.h>
#include <assert.h>

#include <stdlib.h>
#include "move_sort.h"

#include <android/log.h>
#define APPNAME "Search"

void Search::recordHash(int flag, int vl, int depth, int mv) {
  Entry *hash = getHashItem();
  if (hash->depth > depth) {
    return;
  }
  hash->flag = flag;
  hash->depth = depth;
  if (vl > WIN_VALUE) {
    if (mv == 0 && vl <= BAN_VALUE) {
      return;
    }
    hash->vl = vl + pos->distance;
  } else if(vl < -WIN_VALUE) {
    if (mv == 0 && vl >= -BAN_VALUE) {
      return;
    }
    hash->vl = vl - pos->distance;
  } else if (vl == pos->drawValue() && mv == 0) {
    return;
  } else {
    hash->vl = vl;
  }
  hash->mv = mv;
  hash->zobristLock = pos->zobristLock;
}

int Search::probeHash(int vlAlpha, int vlBeta, int depth, std::vector<int> &mv) {
  Entry *hash = getHashItem();
  if (hash->zobristLock != pos->zobristLock) {
    mv[0] = 0;
    return -MATE_VALUE;    
  }

  mv[0] = hash->mv;
  bool mate = false;
  if (hash->vl > WIN_VALUE) {
    if (hash->vl <= BAN_VALUE) {
      return -MATE_VALUE;
    }
    hash->vl -= pos->distance;
    mate = true;
  } else if (hash->vl < -WIN_VALUE) {
    if (hash->vl >= -BAN_VALUE) {
      return -MATE_VALUE;
    }
    hash->vl += pos->distance;
    mate = true;
  } else if (hash->vl == pos->drawValue()) {
    return -MATE_VALUE;
  }

  if (hash->depth < depth && !mate) {
    return -MATE_VALUE;
  }
  if (hash->flag == HASH_BETA) {
    return (hash->vl >= vlBeta) ? hash->vl : -MATE_VALUE;
  }
  if (hash->flag == HASH_ALPHA) {
    return (hash->vl <= vlAlpha) ? hash->vl : -MATE_VALUE;
  }
  return hash->vl;
}


void Search::setBestMove(int mv, int depth) {
  historyTable[pos->historyIndex(mv)] += depth * depth;
  EntryK &mvsKiller = killerTable[pos->distance];
  if (mvsKiller.i != mv) {
    mvsKiller.j = mvsKiller.i;
    mvsKiller.i = mv;  
  }
}



int Search::searchQuiesc(int vlAlpha_, int vlBeta) {
  int mvs_length = -1;
  // printf("CFU searchQuiesc vlAlpha_ %d, vlBeta %d allNodes %d\n", vlAlpha_, vlBeta, allNodes);
  int vlAlpha = vlAlpha_;
  allNodes++;
  int vl = pos->mateValue();
  // printf("CFU vl = %d\n", vl);
  if (vl >= vlBeta) {
    return vl;
  }
  int vlRep = pos->repStatus(1);
  // printf("CFU vlRep = %d\n", vlRep);
  if (vlRep > 0) {
    int temp = pos->repValue(vlRep);
    // printf("CFU repValue %d\n", temp);
    return temp;
  }
  if (pos->distance == LIMIT_DEPTH) {
    int temp = pos->evaluate();
    //printf("CFU evaluate %d\n", temp);
    return temp;
  }

  int vlBest = -MATE_VALUE;
  std::vector<int> mvs, vls;
  if (pos->inCheck()) {
    pos->generateMoves(mvs);
    for (auto const& x: mvs) {
      // printf("CFU %d %d %d\n", x, pos->historyIndex(x), historyTable[pos->historyIndex(x)]);
      vls.push_back(historyTable[pos->historyIndex(x)]);
    }
    shellSort(mvs, vls);
  } else {
    vl = pos->evaluate();
    if (vl > vlBest) {
      if (vl >= vlBeta) {
        return vl;
      }
      vlBest = vl;
      vlAlpha = (vl > vlAlpha) ?  vl : vlAlpha;
    }
    pos->generateMoves(mvs, vls);
    shellSort(mvs, vls);
    for (int i = 0; i < mvs.size(); i++) {
      if (vls[i] < 10 || (vls[i] < 20 && pos->HOME_HALF(pos->DST(mvs[i]), pos->sdPlayer))) {
        mvs_length = i;
        break;
      }
    }
  }
  if (mvs_length == -1) {
    mvs_length = mvs.size();
  }
  for (int i = 0; i < mvs_length; i++) {
    if (!pos->makeMove(mvs[i])) {
      continue;
    }
    vl = -searchQuiesc(-vlBeta, -vlAlpha);
    pos->undoMakeMove();
    if (vl > vlBest) {
      if (vl >= vlBeta) {
        return vl;
      }
      vlBest = vl;
      vlAlpha = (vl > vlAlpha) ? vl : vlAlpha;
    }
  }

  return (vlBest == -MATE_VALUE) ? pos->mateValue() : vlBest;
}

int Search::searchFull(int vlAlpha_, int vlBeta, int depth, bool noNull) {
  // printf("CFU searchFull vlAlpha_ %d, vlBeta %d, depth %d, noNull %d\n", vlAlpha_, vlBeta, depth, noNull);
  int vlAlpha = vlAlpha_;
  if (depth <= 0) {
    return searchQuiesc(vlAlpha, vlBeta);
  }

  allNodes++;
  int vl = pos->mateValue();
  if (vl >= vlBeta) {
    return vl;
  }

  int vlRep = pos->repStatus(1);
  if (vlRep > 0) {
    return pos->repValue(vlRep);
  }

  std::vector<int> mvHash;
  mvHash.push_back(0);
  vl = probeHash(vlAlpha, vlBeta, depth, mvHash);
  if (vl > -MATE_VALUE) {
    return vl;
  }
  if (pos->distance == LIMIT_DEPTH) {
    return pos->evaluate();
  }

  if (!noNull && !pos->inCheck() && pos->nullOkay()) {
    pos->nullMove();
    vl = -searchFull(-vlBeta, 1 - vlBeta, depth - NULL_DEPTH - 1, true);
    pos->undoNullMove();
    if (vl >= vlBeta && (pos->nullSafe() ||
      searchFull(vlAlpha, vlBeta, depth - NULL_DEPTH, true) >= vlBeta)) {
      return vl;
    }
  }

  int hashFlag = HASH_ALPHA;
  int vlBest = -MATE_VALUE;
  int mvBest = 0;
  MoveSort sort(mvHash[0], pos, killerTable, historyTable);
  int mv;
  while ((mv = sort.next()) > 0) {
    if (!pos->makeMove(mv)) {
      continue;
    }
    int newDepth = (pos->inCheck() || sort.singleReply) ? depth : depth - 1;
    if (vlBest == -MATE_VALUE) {
      vl = -searchFull(-vlBeta, -vlAlpha, newDepth, false);
    } else {
      vl = -searchFull(-vlAlpha - 1, -vlAlpha, newDepth, false);
      if (vl > vlAlpha && vl < vlBeta) {
        vl = -searchFull(-vlBeta, -vlAlpha, newDepth, false);
      }
    }
    pos->undoMakeMove();
    if (vl > vlBest) {
      vlBest = vl;
      if (vl >= vlBeta) {
        hashFlag = HASH_BETA;
        mvBest = mv;
        break;
      }
      if (vl > vlAlpha) {
        vlAlpha = vl;
        hashFlag = HASH_PV;
        mvBest = mv;
      }
    }
  }
  if (vlBest == -MATE_VALUE) {
    return pos->mateValue();
  }

  recordHash(hashFlag, vlBest, depth, mvBest);
  if (mvBest > 0) {
    setBestMove(mvBest, depth);
  }
  return vlBest;
}

int Search::searchRoot(int depth) {
  //printf("searchRoot %d\n", depth);
  int vlBest = -MATE_VALUE;
  MoveSort sort(mvResult, pos, killerTable, historyTable);
  int mv;

  while ((mv = sort.next()) > 0) {
    // printf("CFU mv %d\n", mv);
    if (!pos->makeMove(mv)) {
      continue;
    }
    int newDepth = pos->inCheck() ? depth : depth - 1;

    int vl;
    if (vlBest == -MATE_VALUE) {
      vl = -searchFull(-MATE_VALUE, MATE_VALUE, newDepth, true);
    } else {
      vl = -searchFull(-vlBest - 1, -vlBest, newDepth, false);
      if (vl > vlBest) {
        vl = -searchFull(-MATE_VALUE, -vlBest, newDepth, true);
      }
    }

    pos->undoMakeMove();
    if (vl > vlBest) {
      vlBest = vl;
      mvResult = mv;
      if (vlBest > -WIN_VALUE && vlBest < WIN_VALUE) {
        vlBest += (((unsigned int)rand()) & (RANDOMNESS - 1)) - (((unsigned int)rand()) & (RANDOMNESS - 1));
        vlBest = (vlBest == pos->drawValue()) ?  vlBest - 1 : vlBest;
      }
    }
  }
  setBestMove(mvResult, depth);
  return vlBest;
}

bool Search::searchUnique(int vlBeta, int depth) {
  MoveSort sort(mvResult, pos, killerTable, historyTable);
  sort.next();
  int mv;
  while ((mv = sort.next()) > 0) {
    if (!pos->makeMove(mv)) {
      continue;
    }
    int vl = -searchFull(-vlBeta, 1 - vlBeta,
                         pos->inCheck() ? depth : depth - 1, false);
    pos->undoMakeMove();
    if (vl >= vlBeta) {
      return false;
    }
  }
  return true;
}


int Search::searchMain0() {
  mvResult = pos->bookMove();
  if (mvResult > 0) {
    pos->makeMove(mvResult);
    if (pos->repStatus(3) == 0) {
      pos->undoMakeMove();
      return mvResult;
    }
    pos->undoMakeMove();
  }
  return 0;
}


int Search::searchMain1() {
  int i;
  for (i = 0; i <= hashMask; i++) {
    hashTable[i].depth = 0;
    hashTable[i].flag = 0;
    hashTable[i].vl = 0;
    hashTable[i].mv = 0;
    hashTable[i].zobristLock = 0;
  }

  for (i = 0; i < LIMIT_DEPTH; i++) {
    killerTable[i].i = 0;
    killerTable[i].j = 0;
  }

  for (i = 0; i < 4096; i++) {
    historyTable[i] = 0;
  }

  mvResult = 0;
  allNodes = 0;
  pos->distance = 0;

  clock_gettime(CLOCK_MONOTONIC, &my_start);
  STOP_SEARCH = false;
  return 0;
}

int Search::searchMain2(int depth, int millis) {

    if (STOP_SEARCH) {
        return 0;
    }

    struct timespec end;
    int vl = searchRoot(depth);
    //assert(0);

    clock_gettime(CLOCK_MONOTONIC, &end);
    int diff = (end.tv_sec - my_start.tv_sec) * 1000 +  (end.tv_nsec - my_start.tv_nsec) / 1000000;

    //printf("end %d %d diff = %d\n", end.tv_sec, end.tv_nsec, diff);
    if (diff > millis) {
        STOP_SEARCH = true;
        //__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "depth %d end %d %d diff = %d TIMES IS UP\n", depth, (int)end.tv_sec, (int)end.tv_nsec, diff);
        return mvResult;;
    }
    //__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "depth %d end %d %d diff = %d\n", depth, (int)end.tv_sec, (int)end.tv_nsec, diff);

    if (vl > WIN_VALUE || vl < -WIN_VALUE) {
        STOP_SEARCH = true;
        return mvResult;
    }

    if (searchUnique(1 - WIN_VALUE, depth)) {
        STOP_SEARCH = true;
        return mvResult;;
    }

    return mvResult;
}

#if 0
int Search::searchMain(int depth, int millis) {

  int i;
  mvResult = pos->bookMove();
  if (mvResult > 0) {
    pos->makeMove(mvResult);
    if (pos->repStatus(3) == 0) {
      pos->undoMakeMove();
      return mvResult;
    }
    pos->undoMakeMove();
  }

  for (i = 0; i <= hashMask; i++) {
    hashTable[i].depth = 0;
    hashTable[i].flag = 0;
    hashTable[i].vl = 0;
    hashTable[i].mv = 0;
    hashTable[i].zobristLock = 0;
  }

  for (i = 0; i < LIMIT_DEPTH; i++) {
    killerTable[i].i = 0;
    killerTable[i].j = 0;
  }

  for (i = 0; i < 4096; i++) {
    historyTable[i] = 0;
  }

  mvResult = 0;
  allNodes = 0;
  pos->distance = 0;

  //printf("millis = %d\n", millis);
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);
  //printf("start %d %d\n", start.tv_sec, start.tv_nsec);
  for (i = 1; i <= depth; i++) {
    int vl = searchRoot(i);
    //assert(0);

    clock_gettime(CLOCK_MONOTONIC, &end);
    int diff = (end.tv_sec - start.tv_sec) * 1000 +  (end.tv_nsec - start.tv_nsec) / 1000000;
    
    //printf("end %d %d diff = %d\n", end.tv_sec, end.tv_nsec, diff);
    //__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "end %d %d diff = %d\n", (int)end.tv_sec, (int)end.tv_nsec, diff);
    if (diff > millis)
      break;

    if (vl > WIN_VALUE || vl < -WIN_VALUE) {
      break;
    }
    if (searchUnique(1 - WIN_VALUE, i)) {
      break;
    }
  }

  return mvResult;
}
#endif
