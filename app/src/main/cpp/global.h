/*
global.h - Copyright (c) 2020 IcebergFu's Software, www.icebergfu.com

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


#ifndef INCLUDED_GLOBAL_H
#define INCLUDED_GLOBAL_H

#define MATE_VALUE 10000
#define BAN_VALUE (MATE_VALUE - 100)
#define WIN_VALUE (MATE_VALUE - 200)
#define NULL_SAFE_MARGIN 400
#define NULL_OKAY_MARGIN 200
#define DRAW_VALUE 20
#define ADVANCED_VALUE 3

#define RANDOMNESS 8
#define NULL_DEPTH 2

#define HASH_ALPHA 1
#define HASH_BETA 2
#define HASH_PV 3

#define PHASE_HASH 0
#define PHASE_KILLER_1 1
#define PHASE_KILLER_2 2
#define PHASE_GEN_MOVES 3
#define PHASE_REST 4

struct Entry {
  int depth;
  int flag;
  int vl;
  int mv;
  int zobristLock;
};

struct EntryK {
  int i;
  int j;
};

#endif
