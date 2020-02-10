/*
native-lib.cpp - Copyright (c) 2020 IcebergFu's Software, www.icebergfu.com

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


#include <jni.h>
#include <string>
#include <thread>
#include <future>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "book.h"
#include "board.h"
#include "search.h"

#define FIXED_SEED false

#if 0
char STARTUP_FEN0[] = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w";
char STARTUP_FEN1[] = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKAB1R w";
char STARTUP_FEN2[] = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/R1BAKAB1R w";
char STARTUP_FEN3[] = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/9/1C5C1/9/RN2K2NR w";
char *STARTUP_FEN[4] = { STARTUP_FEN0, STARTUP_FEN1, STARTUP_FEN2,  STARTUP_FEN3};
#endif

//int selMoveMode = 2; // 00, Human, Human
// 01, 1st player:Computer, Human
// 10, Human, 2nd player: Computer
// 11, Computer, Computer
//int p0SelLevel = 3; // 0, 1, 2 (1 sec), 3 (10 sec)
//int p1SelLevel = 3; // 0, 1, 2 (1 sec), 3 (10 sec)
//int selHandicap = 0; // 0, 1, 2, 3
Board *board = NULL;

#if 0
extern "C" JNIEXPORT jstring JNICALL
Java_com_icebergfu_chinesechess_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "N";
    return env->NewStringUTF(hello.c_str());
}
#endif

#if 0
extern "C" JNIEXPORT jboolean JNICALL
Java_com_icebergfu_chinesechess_MainActivity_passBooleanFromJNI(
        JNIEnv *env,
        jobject /* this */,
        jboolean bool1) {

    return bool1;
}
#endif

extern "C" JNIEXPORT jboolean JNICALL
Java_com_icebergfu_chinesechess_MainActivity_convertMoveFromJNI(
        JNIEnv *env,
        jobject /* this */,
        jint move,
        jboolean isComputer) {
    return (jboolean)(board->addMove(move, (bool)isComputer));
}


extern "C" JNIEXPORT jint JNICALL
Java_com_icebergfu_chinesechess_MainActivity_gameResultFromJNI(
        JNIEnv *env,
        jobject /* this */
) {
    return (jint)board->result;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_icebergfu_chinesechess_MainActivity_undoFromJNI(
        JNIEnv *env,
        jobject /* this */
) {
    board->result = RESULT_UNKNOWN;
    board->pos->undoMakeMove();
    return (jboolean)false;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_icebergfu_chinesechess_MyAsyncTask_computerMove0FromJNI(
        JNIEnv *env,
        jobject /* this */
) {
    int this_move = board->search->searchMain0();
    if (this_move != 0) {
        board->addMove(this_move, true);
        return (jint)this_move;
    }
    return (jint)this_move;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_icebergfu_chinesechess_MyAsyncTask_computerMove1FromJNI(
        JNIEnv *env,
        jobject /* this */
) {
    int result = board->search->searchMain1();
    return (jint)result;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_icebergfu_chinesechess_MyAsyncTask_computerMove2FromJNI(
        JNIEnv *env,
        jobject /* this */,
        jint depth
) {
    int this_move = board->search->searchMain2(depth, board->pos->sdPlayer == 0 ? board->millis0 : board->millis1);
    return (jint)this_move;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_icebergfu_chinesechess_MyAsyncTask_addMoveFromJNI(
        JNIEnv *env,
        jobject /* this */,
        jint move) {
    return (jboolean)(board->addMove(move, true));
}

/*
extern "C" JNIEXPORT jint JNICALL
Java_com_icebergfu_chinesechess_MyAsyncTask_computerMoveFromJNI(
        JNIEnv *env,
        jobject
) {
    int this_move = board->search->searchMain(LIMIT_DEPTH, board->pos->sdPlayer == 0 ? board->millis0 : board->millis1);
    board->addMove(this_move, true);
    return (jint)this_move;
}
*/
/*
void func(Board *b, std::promise<int> && p) {
    int this_move = b->search->searchMain(LIMIT_DEPTH, b->pos->sdPlayer == 0 ? b->millis0 : b->millis1);
    b->addMove(this_move, true);
    p.set_value(this_move);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_icebergfu_chinesechess_MainActivity_computerMoveFromJNI(
        JNIEnv *env,
        jobject
) {

    //int this_move = board->search->searchMain(LIMIT_DEPTH, board->pos->sdPlayer == 0 ? board->millis0 : board->millis1);
    //board->addMove(this_move, true);
    //return (jint)this_move;


    std::promise<int> p;
    auto f = p.get_future();
    std::thread t(&func, board, std::move(p));
    t.join();
    int i = f.get();
    return (jint)i;
}
*/

int MY_INIT_ONCE = 1;

extern "C" JNIEXPORT jboolean JNICALL
Java_com_icebergfu_chinesechess_MainActivity_initAllFromJNI(
        JNIEnv *env,
        jobject /* this */,
        jint move_mode,
        jint p0_level,
        jint p1_level
) {
    if (MY_INIT_ONCE == 1) {
        int seed = time(NULL);
#if FIXED_SEED == true
        seed = 0;
#endif
        //printf("seed is %d\n", seed);
        srand(seed);
        setup_keys();
        MY_INIT_ONCE = 0;
    }

    if (board != NULL) {
        // CFU!!! We allow keeping all previous board.
        // delete board;
    }

    board = new Board();
    board->setSearch(16);

    int p0SelLevel = p0_level;
    int p1SelLevel = p1_level;
    board->millis0 = (int)powf(10.0f, (float)(p0SelLevel + 1));
    board->millis1 = (int)powf(10.0f, (float)(p1SelLevel + 1));
    //int selMoveMode = move_mode;
    //board->computer = selMoveMode;

    return (jboolean)false;
}
