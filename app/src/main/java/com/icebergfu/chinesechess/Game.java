/*
Game.java - Copyright (c) 2020 IcebergFu's Software, www.icebergfu.com

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

package com.icebergfu.chinesechess;

public class Game {

    public class Loc {
        public int x;
        public int y;

        public Loc(int i, int j) {
            x = i;
            y = j;
        }
    }

    public final int TOTAL_PIECES = 32;
    public final static int WIDTH = 9;
    public final static int HEIGHT = 10;

    public Loc all[] = new Loc[TOTAL_PIECES];
    public int board[][];

    public final int P_G0 = 0;
    public final int P_A0 = 1;
    public final int P_A1 = 2;
    public final int P_E0 = 3;
    public final int P_E1 = 4;
    public final int P_H0 = 5;
    public final int P_H1 = 6;
    public final int P_C0 = 7;
    public final int P_C1 = 8;
    public final int P_P0 = 9;
    public final int P_P1 = 10;
    public final int P_S0 = 11;
    public final int P_S1 = 12;
    public final int P_S2 = 13;
    public final int P_S3 = 14;
    public final int P_S4 = 15;
    public final int P_g0 = 16;
    public final int P_a0 = 17;
    public final int P_a1 = 18;
    public final int P_e0 = 19;
    public final int P_e1 = 20;
    public final int P_h0 = 21;
    public final int P_h1 = 22;
    public final int P_c0 = 23;
    public final int P_c1 = 24;
    public final int P_p0 = 25;
    public final int P_p1 = 26;
    public final int P_s0 = 27;
    public final int P_s1 = 28;
    public final int P_s2 = 29;
    public final int P_s3 = 30;
    public final int P_s4 = 31;

    public void reset() {
        int i;
        for (i = 0; i < TOTAL_PIECES;i++){
            all[i] = new Loc(-1, -1);
        }
        board = new int[WIDTH][HEIGHT];
        int x, y;
        for (x = 0; x <WIDTH; x++) {
            for (y = 0; y < HEIGHT; y++) {
                board[x][y] = -1;
            }
        }
    }

    public void default_pos() {
        int x, y;

        all[P_G0].x = 4; all[P_G0].y = 0;
        all[P_A0].x = 3; all[P_A0].y = 0;
        all[P_A1].x = 5; all[P_A1].y = 0;
        all[P_E0].x = 2; all[P_E0].y = 0;
        all[P_E1].x = 6; all[P_E1].y = 0;
        all[P_H0].x = 1; all[P_H0].y = 0;
        all[P_H1].x = 7; all[P_H1].y = 0;

        all[P_C0].x = 0; all[P_C0].y = 0;
        all[P_C1].x = 8; all[P_C1].y = 0;
        all[P_P0].x = 1; all[P_P0].y = 2;
        all[P_P1].x = 7; all[P_P1].y = 2;
        all[P_S0].x = 0; all[P_S0].y = 3;
        all[P_S1].x = 2; all[P_S1].y = 3;
        all[P_S2].x = 4; all[P_S2].y = 3;
        all[P_S3].x = 6; all[P_S3].y = 3;
        all[P_S4].x = 8; all[P_S4].y =3;

        all[P_g0].x = 4; all[P_g0].y = 9;
        all[P_a0].x = 5; all[P_a0].y = 9;
        all[P_a1].x = 3; all[P_a1].y = 9;
        all[P_e0].x = 6; all[P_e0].y = 9;
        all[P_e1].x = 2; all[P_e1].y = 9;
        all[P_h0].x = 7; all[P_h0].y = 9;
        all[P_h1].x = 1; all[P_h1].y = 9;
        all[P_c0].x = 0; all[P_c0].y = 9;
        all[P_c1].x = 8; all[P_c1].y = 9;
        all[P_p0].x = 7; all[P_p0].y = 7;
        all[P_p1].x = 1; all[P_p1].y = 7;
        all[P_s0].x = 8; all[P_s0].y = 6;
        all[P_s1].x = 6; all[P_s1].y = 6;
        all[P_s2].x = 4; all[P_s2].y = 6;
        all[P_s3].x = 2; all[P_s3].y = 6;
        all[P_s4].x = 0; all[P_s4].y = 6;

        int i;
        for (i = 0; i < TOTAL_PIECES; i++) {
            board[all[i].x][all[i].y] = i;
        }

    }

    public void move_piece(int index, int temp[]) {
        int old_x = all[index].x;
        int old_y = all[index].y;
        if (old_x >=0 & old_y >= 0) {
            board[old_x][old_y] = -1;
        }
        all[index].x = temp[0];
        all[index].y = temp[1];
        board[temp[0]][temp[1]] = index;
    }

    public void put_piece(int piece, int x, int y) {
        if (x == -1 && y == -1){
            if (piece > -1) {
                all[piece].x = x;
                all[piece].y = y;
            }
        }
        else {
            board[x][y] = piece;
            if (piece > -1) {
                all[piece].x = x;
                all[piece].y = y;
            }
        }
    }
}
