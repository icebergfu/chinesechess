/*
MainActivity.java - Copyright (c) 2020 IcebergFu's Software, www.icebergfu.com

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

import androidx.appcompat.app.AppCompatActivity;

//import android.content.res.Resources;
//import android.content.res.TypedArray;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
//import android.graphics.Typeface;
import android.graphics.PorterDuff;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.Log;
//import android.util.TypedValue;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuInflater;
//import android.view.MenuItem;
import android.view.MenuItem;
import android.view.View;
//import android.view.ViewGroup;
import android.widget.Button;
import android.widget.Toast;

//import java.io.BufferedReader;
import java.io.FileNotFoundException;
//import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;


public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    //public native String stringFromJNI();
    //public native boolean passBooleanFromJNI(boolean bool1);
    public native boolean convertMoveFromJNI(int move, boolean isComputer);
    public native int gameResultFromJNI();
    //public native int computerMoveFromJNI();
    public native boolean initAllFromJNI(int move_mode, int p0_level, int p1_level);
    public native boolean undoFromJNI();

    public static final int OPTION_REQUEST = 1;

    //private int primaryColor;
    //private int highlightColor;
    private static final String TAG_VALUE = com.icebergfu.chinesechess.MainActivity.class.getSimpleName();
    private static final String filename = "config.txt";
    private int m_from_x, m_from_y; //, m_to_x, m_to_y;
    Button m_from_button, m_to_button;
    private String m_from_string;
    private int m_from_piece, m_to_piece;
    private int current_color;
    private int current_player;
    private int move_mode = 2; // 00 human human; 01 computer human; 10 human computer; 11 computer computer
    private int p0_level = 0;
    private int p1_level = 0;
    private int data_destroyed;

    private boolean computer_is_running;
    MyAsyncTask m_task;

    private int game_state;
    private int num_of_history;
    private int index_of_history;
    private int num_of_redo;
    private final int max_of_history = 256;
    private History []history = new History[max_of_history];
    private final int color_mask = 0x008080;
    private final int red_color_mask = 0xff0000;
    //private boolean enable_hints;
    //private int num_of_hints;
    //private final int max_of_hints = 18;
    //private Hint []hint_array = new Hint[max_of_hints];
    private Game game;

    MenuItem item_alarm;
    private int m_alarm_color = Color.WHITE;

    public void create_space() {

        int i;
        for (i = 0; i < max_of_history; i++) {
            history[i] = new History();
        }

        //for (i = 0; i < max_of_hints; i++) {
         //   hint_array[i] = new Hint();
        //}

        game = new Game();
    }

    private int[] buttonIDs = new int[]{
            R.id.button00, R.id.button10, R.id.button20, R.id.button30, R.id.button40, R.id.button50, R.id.button60, R.id.button70, R.id.button80,
            R.id.button01, R.id.button11, R.id.button21, R.id.button31, R.id.button41, R.id.button51, R.id.button61, R.id.button71, R.id.button81,
            R.id.button02, R.id.button12, R.id.button22, R.id.button32, R.id.button42, R.id.button52, R.id.button62, R.id.button72, R.id.button82,
            R.id.button03, R.id.button13, R.id.button23, R.id.button33, R.id.button43, R.id.button53, R.id.button63, R.id.button73, R.id.button83,
            R.id.button04, R.id.button14, R.id.button24, R.id.button34, R.id.button44, R.id.button54, R.id.button64, R.id.button74, R.id.button84,
            R.id.button05, R.id.button15, R.id.button25, R.id.button35, R.id.button45, R.id.button55, R.id.button65, R.id.button75, R.id.button85,
            R.id.button06, R.id.button16, R.id.button26, R.id.button36, R.id.button46, R.id.button56, R.id.button66, R.id.button76, R.id.button86,
            R.id.button07, R.id.button17, R.id.button27, R.id.button37, R.id.button47, R.id.button57, R.id.button67, R.id.button77, R.id.button87,
            R.id.button08, R.id.button18, R.id.button28, R.id.button38, R.id.button48, R.id.button58, R.id.button68, R.id.button78, R.id.button88,
            R.id.button09, R.id.button19, R.id.button29, R.id.button39, R.id.button49, R.id.button59, R.id.button69, R.id.button79, R.id.button89,
    };

    /*
    private int[] stringIDs2 = new int[] {
      R.string.G0, R.string.A0, R.string.A1, R.string.E0, R.string.E1, R.string.H0, R.string.H1,
            R.string.C0, R.string.C1, R.string.P0, R.string.P1, R.string.S0, R.string.S1,
            R.string.S2, R.string.S3, R.string.S4,
            R.string.g0, R.string.a0, R.string.a1, R.string.e0, R.string.e1, R.string.h0, R.string.h1,
            R.string.c0, R.string.c1, R.string.p0, R.string.p1, R.string.s0, R.string.s1,
            R.string.s2, R.string.s3, R.string.s4,
    };
*/
    private int[] stringIDs = new int[] {
            R.string.G02, R.string.A02, R.string.A02, R.string.E02, R.string.E02, R.string.H02, R.string.H02,
            R.string.C02, R.string.C02, R.string.P02, R.string.P02, R.string.S02, R.string.S02,
            R.string.S02, R.string.S02, R.string.S02,
            R.string.g02, R.string.a02, R.string.a02, R.string.e02, R.string.e02, R.string.h02, R.string.h02,
            R.string.c02, R.string.c02, R.string.p02, R.string.p02, R.string.s02, R.string.s02,
            R.string.s02, R.string.s02, R.string.s02,
    };

    private int[] nameIDs = new int[] {
            R.string.first_label,
            R.string.second_label,
            R.string.third_label,
            R.string.fourth_label,
            R.string.fifth_label,
    };

    private void init_game() {
        data_destroyed = 0;
        current_color = red_color_mask;
        current_player = 0;

        computer_is_running = false;


        game_state = 0;
        num_of_history = 0;
        index_of_history = 0;
        num_of_redo = 0;
        //enable_hints = true;
        game.reset();

        //initAllFromJNI(move_mode, p0_level, p1_level);
    }

    private int change_piece(Button button) {
        int c = button.getTextColors().getDefaultColor();

        if ((c & color_mask) != color_mask) {
            button.setTextColor(c + color_mask);
            return c + color_mask;
        }
        else {
            button.setTextColor(c - color_mask);
            return c - color_mask;
        }
    }

    private void update_buttons() {
        int x, y;
        for (y = 0; y < Game.HEIGHT; y++) {
            for (x = 0; x < Game.WIDTH; x++) {
              int offset = y * Game.WIDTH + x;
                int button_id = buttonIDs[offset];
                Button button = findViewById(button_id);
                //button.setTypeface(button.getTypeface(), Typeface.BOLD);
                int p = game.board[x][y];
                if (p == -1) {
                    button.setText("");
                }
                else {
                    String s0 = getResources().getString(stringIDs[p]);
                    /*
                    String s1= String.valueOf(passBooleanFromJNI(false));
                    String s2 = stringFromJNI();
                    String s3 = String.valueOf(convertMoveFromJNI(1234));
                    */
                    button.setText(s0);
                    if (p < game.P_g0)
                        button.setTextColor(Color.RED);
                    else
                        button.setTextColor(Color.BLACK);
                }
            }
        }
    }

/*
    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        int i;
        for (i = 0; i < game.TOTAL_PIECES; i++) {
            outState.putIntArray("p"  + i, new int[]{game.all[i].x, game.all[i].y});
        }

        outState.putInt("move_mode", move_mode);
        outState.putInt("p0_level", p0_level);
        outState.putInt("p1_level", p1_level);
        outState.putBoolean("enable_hints", enable_hints);
        outState.putInt("num_of_redo", num_of_redo);
        outState.putInt("num_of_history", num_of_history);
        outState.putInt("index_of_history", index_of_history);
        int j = index_of_history - num_of_history;
        if (j < 0) {
            j = j + max_of_history;
        }
        for (i = 0; i < num_of_history + num_of_redo; i++) {
            outState.putIntArray("h" + j, new int[]{history[j].from_x, history[j].from_y,
                    history[j].from_piece, history[j].to_x, history[j].to_y, history[j].to_piece});
            j = j + 1;
            if (j == max_of_history) {
                j = 0;
            }
        }
    }
*/

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        /*
        TypedValue typedValue = new TypedValue();
        Resources.Theme theme = this.getTheme();
        theme.resolveAttribute(android.R.attr.textColorPrimary, typedValue, true);
        TypedArray arr = this.obtainStyledAttributes(typedValue.data, new int[]{
                android.R.attr.textColorPrimary});
        primaryColor = arr.getColor(0, Color.BLACK);
        arr = this.obtainStyledAttributes(typedValue.data, new int[]{
                android.R.attr.textColorHighlight});
        highlightColor = arr.getColor(0, Color.RED);
        arr.recycle();
        primaryColor = Color.BLACK;
        highlightColor = Color.RED;*/

        create_space();
        init_game();
        /*
        if (savedInstanceState != null) {
            int i;
            for (i = 0; i < game.TOTAL_PIECES; i++) {
                int[] temp = savedInstanceState.getIntArray("p" + i);
                game.put_piece(i, temp[0], temp[1]);
            }

            move_mode = savedInstanceState.getInt("move_mode", 2);
            p0_level = savedInstanceState.getInt("p0_level", 0);
            p1_level = savedInstanceState.getInt("p1_level", 0);
            enable_hints = savedInstanceState.getBoolean("enable_hints", true);
            num_of_redo = savedInstanceState.getInt("num_of_redo", 0);
            num_of_history = savedInstanceState.getInt("num_of_history", 0);
            index_of_history = savedInstanceState.getInt("index_of_history", 0);
            int j = index_of_history - num_of_history;
            if (j < 0) {
                j = j + max_of_history;
            }
            for (i = 0; i < num_of_history + num_of_redo; i++) {
                int[] temp = savedInstanceState.getIntArray("h" + j);
                history[j].from_x = temp[0];
                history[j].from_y = temp[1];
                history[j].from_piece = temp[2];
                history[j].to_x = temp[3];
                history[j].to_y = temp[4];
                history[j].to_piece = temp[5];

                j = j + 1;
                if (j == max_of_history) {
                    j = 0;
                }
            }
        }
        else */
        if (read_data()) {
            // At this point, we have move_mode, p0_level, p1_level
            initAllFromJNI(move_mode, p0_level, p1_level);

            game.default_pos();

            int i;
            int j = index_of_history - num_of_history;
            if (j < 0) {
                j = j + max_of_history;
            }
            for (i = 0; i < num_of_history; i++) {

                m_from_x = history[j].from_x;
                m_from_y = history[j].from_y;
                m_from_piece = history[j].from_piece;
                int to_x = history[j].to_x;
                int to_y = history[j].to_y;

                game.put_piece(-1, m_from_x, m_from_y);
                game.put_piece(m_from_piece, to_x, to_y);

                int FILE_LEFT = 3;
                int RANK_TOP = 3;
                int from_sq = FILE_LEFT + m_from_x + ((RANK_TOP + (9 - m_from_y)) << 4);
                int to_sq = FILE_LEFT + to_x + ((RANK_TOP + (9 - to_y)) << 4);
                int move = from_sq + (to_sq << 8);
                convertMoveFromJNI(move, ((1<<current_player) & move_mode) != 0);

                current_color = current_color ^ red_color_mask;
                current_player = 1 - current_player;

                j = j + 1;
                if (j == max_of_history) {
                    j = 0;
                }
            }

        } else {
            initAllFromJNI(move_mode, p0_level, p1_level);

            game.default_pos();
        }
        update_buttons();

        /*
        if (((1 << current_player) & move_mode) != 0) {
            // Let the computer play automatically
            let_computer_run();
        }
        */


    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu_main, menu);
        item_alarm = menu.getItem(3);
        update_alarm(false);
        return true;
    }

    String player_name(int i) {
        if (i == 0) {
            if ((move_mode&1) == 0) {
                return getResources().getString(nameIDs[0]);
            }
            return getResources().getString(nameIDs[1+p0_level]);
        }

        if ((move_mode&2) == 0) {
            return getResources().getString(nameIDs[0]);
        }
        return getResources().getString(nameIDs[1+p1_level]);
    }

    public void do_toast(int result) {
        if (result == 1) {
            String name = player_name(1 - current_player);
            Toast toast = Toast.makeText(this, "Game Over\nPlayer " + (1 - current_player) + " (" + name + ") WON!", Toast.LENGTH_SHORT);
            toast.setGravity(Gravity.CENTER_VERTICAL | Gravity.CENTER_HORIZONTAL, 0, 0);
            toast.show();
        }
        else if (result == 2) {
            Toast toast = Toast.makeText(this, "Game Over\nDRAW", Toast.LENGTH_SHORT);
            toast.setGravity(Gravity.CENTER_VERTICAL | Gravity.CENTER_HORIZONTAL, 0, 0);
            toast.show();
        }
    }

    public void selectPiece(View view) {

        if (computer_is_running) {
            Toast toast = Toast.makeText(this, "Computer is thinking!\nPlease wait!", Toast.LENGTH_SHORT);
            toast.setGravity(Gravity.CENTER_VERTICAL | Gravity.CENTER_HORIZONTAL, 0, 0);
            toast.show();
            return;
        }

        int result = gameResultFromJNI();
        if (result != 0)
        {
            do_toast(result);
            return;
        }

        if (((1 << current_player) & move_mode) != 0) {
            // Let the computer play automatically
            let_computer_run();
            return;
        }

        Button button = (Button) view;
        int id = button.getId();
        int x = 0;
        int y = 0;
        boolean found = false;
        for (; y < 10; y++) {
            for (x = 0; x < 9; x++) {
                int offset = y * 9 + x;
                if (id == buttonIDs[offset]) {
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
        //Log.d(TAG_VALUE, "x = " + x + " y = " + y);
        if (!found)
            return;

        if (game_state == 0) {
            m_from_x = x;
            m_from_y = y;
            m_from_piece = game.board[x][y];

            m_from_button = button;
            m_from_string = m_from_button.getText().toString();
            int from_color = m_from_button.getTextColors().getDefaultColor();

            if (m_from_piece != -1 && (from_color & red_color_mask) == current_color) {
                change_piece(button);
                game_state = 1;

                /*if (enable_hints) {
                    show_hints();
                }*/
            }
        } else if (game_state == 1) {

            m_to_button = button;
            //m_to_string = m_to_button.getText().toString();
            m_to_piece = game.board[x][y];

            int from_color = m_from_button.getTextColors().getDefaultColor();
            int to_color = m_to_button.getTextColors().getDefaultColor();

            if (m_to_piece == -1 || (from_color & red_color_mask) != (to_color & red_color_mask)) {
                /* Check if this is a valid move */
                int FILE_LEFT = 3;
                int RANK_TOP = 3;
                int from_sq = FILE_LEFT + m_from_x + ((RANK_TOP + (9 - m_from_y)) << 4);
                int to_sq = FILE_LEFT + x + ((RANK_TOP + (9 - y)) << 4);
                int move = from_sq + (to_sq << 8);
                boolean is_legal_move = convertMoveFromJNI(move, false);
                if (is_legal_move) {
                    /*if (enable_hints) {
                        remove_hints();
                    }*/

                    m_from_button.setText("");
                    m_to_button.setText(m_from_string);
                    int orig_from_color = change_piece(m_from_button);
                    m_to_button.setTextColor(orig_from_color);
                    game_state = 0;
                    game.put_piece(-1, m_from_x, m_from_y);
                    game.put_piece(m_from_piece, x, y);
                    if (m_to_piece > 0) {
                        game.all[m_to_piece].x = -1;
                        game.all[m_to_piece].y = -1;
                    }
                    add_history(m_from_x, m_from_y, x, y);

                    num_of_redo = 0;
                }
            } else if ((from_color & red_color_mask) == (to_color & red_color_mask)) {
                /*if (enable_hints) {
                    remove_hints();
                }*/

                change_piece(m_from_button);

                m_from_x = x;
                m_from_y = y;
                m_from_piece = game.board[x][y];

                m_from_button = button;
                m_from_string = m_from_button.getText().toString();
                change_piece(button);

                /*if (enable_hints) {
                    show_hints();
                }*/
            }
        }
    }



    private void let_computer_run() {
        //int this_move = computerMoveFromJNI();
        //let_computer_run2(this_move);

        computer_is_running = true;
        m_task = new MyAsyncTask(this);
        m_task.execute();
    }

    public void let_computer_run2(int this_move) {

        //Log.e(TAG_VALUE, "this_move = " + this_move);
        int from_sq = this_move & 0xff;
        int to_sq = (this_move >> 8) & 0xff;
        int c_from_x = (from_sq & 0xf) - 3;
        int c_from_y = 9 - (((from_sq >> 4)&0xf) - 3);
        int c_to_x = (to_sq & 0xf) - 3;
        int c_to_y = 9 - (((to_sq >> 4)&0xf) - 3);
        m_from_button = findViewById(buttonIDs[c_from_x + 9 * c_from_y]);
        m_to_button = findViewById(buttonIDs[c_to_x + 9 * c_to_y]);
        change_piece(m_from_button);

        m_from_string = m_from_button.getText().toString();
        m_from_piece = game.board[c_from_x][c_from_y];
        //m_to_string = m_to_button.getText().toString();
        m_to_piece = game.board[c_to_x][c_to_y];

        m_from_button.setText("");
        m_to_button.setText(m_from_string);
        int orig_from_color = change_piece(m_from_button);
        m_to_button.setTextColor(orig_from_color);
        game_state = 0;
        game.put_piece(-1, c_from_x, c_from_y);
        game.put_piece(m_from_piece, c_to_x, c_to_y);
        if (m_to_piece > 0) {
            game.all[m_to_piece].x = -1;
            game.all[m_to_piece].y = -1;
        }
        add_history(c_from_x, c_from_y, c_to_x, c_to_y);

        num_of_redo = 0;

        computer_is_running = false;
    }

    private void add_history(int from_x, int from_y, int to_x, int to_y) {

        history[index_of_history].from_x = from_x;
        history[index_of_history].from_y = from_y;
        history[index_of_history].from_piece = m_from_piece;
        //history[index_of_history].from_string = from_string;
        //history[index_of_history].from_button = from_button;
        //history[index_of_history].from_color = from_color;
        history[index_of_history].to_x = to_x;
        history[index_of_history].to_y = to_y;
        history[index_of_history].to_piece = m_to_piece;
        //history[index_of_history].to_string = to_string;
        //history[index_of_history].to_button = to_button;
        //history[index_of_history].to_color = to_color;
        index_of_history++;
        num_of_history++;
        if (num_of_history > max_of_history) {
            data_destroyed = 1;
            num_of_history = max_of_history;
        }
        if (index_of_history == max_of_history)
            index_of_history = 0;

        current_color = current_color ^ red_color_mask;
        current_player = 1 - current_player;
        update_alarm(false);

        int result = gameResultFromJNI();
        if (result != 0)
        {
            do_toast(result);
            return;
        }

        if (move_mode == 1 || move_mode == 2) {
            if ((1 << current_player) == move_mode) {
                // Let the computer play automatically
                let_computer_run();
            }
        }
    }


    //private void find_hints() {

        //num_of_hints = 0;
        //if (m_from_string.equals("\uD83E\uDE60")) {
            /*
            hint_array[num_of_hints].x = from_x;
            hint_array[num_of_hints].y = from_y + 1;
            hint_array[num_of_hints].offset = hint_array[num_of_hints].y * 9 + hint_array[num_of_hints].x ;
            num_of_hints++;
             */
        //}
    //}

/*
    private void show_hints() {
        find_hints();
        for (int i = 0; i < num_of_hints; i++) {
            int offset = hint_array[i].offset;
            int id = buttonIDs[offset];
            Button hint_button = findViewById(id);
            //hint_button.setText("┼");
            //hint_button.setTextColor(Color.GRAY);
            hint_button.setBackgroundResource(android.R.drawable.bottom_bar);
        }
    }

    public void remove_hints() {
        for (int i = 0; i < num_of_hints; i++) {
            int offset = hint_array[i].offset;
            int id = buttonIDs[offset];
            Button hint_button = findViewById(id);
            //hint_button.setText("");
            //hint_button.setTextColor(Color.WHITE);
            //hint_button.setBackgroundColor(Color.DKGRAY);
            hint_button.setBackgroundResource(android.R.drawable.btn_default);
        }
        num_of_hints = 0;
    }
*/

    public void undo_move(MenuItem item) {
        if (computer_is_running) {
            Toast toast = Toast.makeText(this, "Computer is thinking!\nPlease wait!", Toast.LENGTH_SHORT);
            toast.setGravity(Gravity.CENTER_VERTICAL | Gravity.CENTER_HORIZONTAL, 0, 0);
            toast.show();
            return;
        }

        if (num_of_history > 0) {


            undoFromJNI();

            if (game_state == 1) {
                change_piece(m_from_button);
                game_state = 0;
            }

            num_of_history--;
            index_of_history--;
            if (index_of_history < 0)
                index_of_history = max_of_history - 1;

            int from_x = history[index_of_history].from_x;
            int from_y = history[index_of_history].from_y;
            int from_piece = history[index_of_history].from_piece;
            //String from_string = history[index_of_history].from_string;
            //Button from_button = history[index_of_history].from_button;
            //int from_color = history[index_of_history].from_color;
            String from_string;
            if (from_piece < 0) {
                from_string = "";
            } else {
                from_string = getResources().getString(stringIDs[from_piece]);
            }

            Button from_button = findViewById(buttonIDs[from_y * Game.WIDTH + from_x]);
                        int from_color;
            if (from_piece < game.P_g0) {
                from_color = Color.RED;
            } else {
                from_color = Color.BLACK;
            }

            int to_x = history[index_of_history].to_x;
            int to_y = history[index_of_history].to_y;
            int to_piece = history[index_of_history].to_piece;
            //String to_string = history[index_of_history].to_string;
            //Button to_button = history[index_of_history].to_button;
            //int to_color = history[index_of_history].to_color;
            String to_string;
            if (to_piece < 0) {
                to_string = "";
            } else {
                to_string = getResources().getString(stringIDs[to_piece]);
            }

            Button to_button = findViewById(buttonIDs[to_y * Game.WIDTH + to_x]);
            int to_color;
            if (to_piece < game.P_g0) {
                to_color = Color.RED;
            } else {
                to_color = Color.BLACK;
            }

            from_button.setTextColor(from_color);
            from_button.setText(from_string);
            to_button.setTextColor(to_color);
            to_button.setText(to_string);

            num_of_redo++;

            game.put_piece(from_piece, from_x, from_y);
            game.put_piece(to_piece, to_x, to_y);

            current_color = current_color ^ red_color_mask;
            current_player = 1 - current_player;
            update_alarm(false);
        }
    }



    public void update_alarm(boolean update) {
        Drawable drawable = item_alarm.getIcon();
        if (drawable != null) {
            drawable.mutate();
            if (update && m_alarm_color != Color.WHITE) {
                m_alarm_color = Color.WHITE;
                drawable.setColorFilter(Color.WHITE, PorterDuff.Mode.SRC_ATOP);
            } else {
                if (current_player == 0) {
                    m_alarm_color = Color.RED;
                    drawable.setColorFilter(Color.RED, PorterDuff.Mode.SRC_ATOP);
                } else {
                    m_alarm_color = Color.BLACK;
                    drawable.setColorFilter(Color.BLACK, PorterDuff.Mode.SRC_ATOP);
                }
            }
        }
    }

    public void on_alarm(MenuItem item) {
        //update_alarm();
    }

    public void redo_move(MenuItem item) {
        if (computer_is_running) {
            Toast toast = Toast.makeText(this, "Computer is thinking!\nPlease wait!", Toast.LENGTH_SHORT);
            toast.setGravity(Gravity.CENTER_VERTICAL | Gravity.CENTER_HORIZONTAL, 0, 0);
            toast.show();
            return;
        }

        if (num_of_redo > 0) {
            if (game_state == 1) {
                change_piece(m_from_button);
                game_state = 0;
            }

            int from_x = history[index_of_history].from_x;
            int from_y = history[index_of_history].from_y;
            int from_piece = history[index_of_history].from_piece;
            //String from_string = history[index_of_history].from_string;
            //Button from_button = history[index_of_history].from_button;
            //int from_color = history[index_of_history].from_color;
            String from_string;
            if (from_piece < 0) {
                from_string = "";
            } else {
                from_string = getResources().getString(stringIDs[from_piece]);
            }

            Button from_button = findViewById(buttonIDs[from_y * Game.WIDTH + from_x]);
            int from_color;
            if (from_piece < game.P_g0) {
                from_color = Color.RED;
            } else {
                from_color = Color.BLACK;
            }
            int to_x = history[index_of_history].to_x;
            int to_y = history[index_of_history].to_y;
            //int to_piece = history[index_of_history].to_piece;
            //String to_string = history[index_of_history].to_string;
            //Button to_button = history[index_of_history].to_button;
            /*
            String to_string;
            if (to_piece < 0) {
                to_string = "";
            } else {
                to_string = getResources().getString(stringIDs[to_piece]);
            }
            */
            Button to_button = findViewById(buttonIDs[to_y * Game.WIDTH + to_x]);

            from_button.setText("");
            to_button.setTextColor(from_color);
            to_button.setText(from_string);

            num_of_history++;
            if (num_of_history > max_of_history)
                num_of_history = max_of_history;

            index_of_history++;
            if (index_of_history == max_of_history)
                index_of_history = 0;

            num_of_redo--;

            game.put_piece(-1, from_x, from_y);
            game.put_piece(from_piece, to_x, to_y);

            int FILE_LEFT = 3;
            int RANK_TOP = 3;
            int from_sq = FILE_LEFT + from_x + ((RANK_TOP + (9 - from_y)) << 4);
            int to_sq = FILE_LEFT + to_x + ((RANK_TOP + (9 - to_y)) << 4);
            int move = from_sq + (to_sq << 8);
            convertMoveFromJNI(move, ((1<<current_player) & move_mode) != 0);

            current_color = current_color ^ red_color_mask;
            current_player = 1 - current_player;
            update_alarm(false);
        }
    }

    public void start_new_game(MenuItem item) {
        if (computer_is_running) {
            Toast toast = Toast.makeText(this, "Computer is thinking!\nPlease wait!", Toast.LENGTH_SHORT);
            toast.setGravity(Gravity.CENTER_VERTICAL | Gravity.CENTER_HORIZONTAL, 0, 0);
            toast.show();
            return;
        }

        int p0 = move_mode & 1;
        int p1 = (move_mode >>1) & 1;
        if (p0 > 0) {
            p0 = p0_level + 1;
        }
        if (p1 > 0) {
            p1 = p1_level + 1;
        }
        int option = p0 + (p1 << 3);

        Intent intent = new Intent(this, Main2Activity.class);
        intent.putExtra(Main2Activity.EXTRA_REPLY, Integer.toString(option));
        startActivityForResult(intent, OPTION_REQUEST);
/*
        setContentView(R.layout.activity_main);
        init_game();
        game.default_pos();
        update_buttons();

        if (move_mode == 1) {
            if ((1 << current_player) == move_mode) {
                // Let the computer play automatically
                let_computer_run();
            }
        }

 */
    }

    @Override
    public void onActivityResult(int requestCode,
                                 int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == OPTION_REQUEST) {
            if (resultCode == RESULT_OK) {
                String reply =
                        data.getStringExtra(Main2Activity.EXTRA_REPLY);
                int option = Integer.parseInt(reply);
                int p0 = option & 7;
                int p1 = (option >> 3) & 7;
                if (p0 <= 4 && p1 <= 4) {
                    move_mode = ((p0 > 0)?1:0) | (((p1 > 0) ? 1 : 0) << 1);
                    p0_level = (p0 == 0) ? 0 : p0 - 1;
                    p1_level = (p1 == 0) ? 0 : p1 - 1;
                }

                setContentView(R.layout.activity_main);
                init_game();
                initAllFromJNI(move_mode, p0_level, p1_level);
                game.default_pos();
                update_buttons();
                update_alarm(false);

                /*
                if (((1 << current_player) & move_mode) != 0) {
                    // Let the computer play automatically
                    let_computer_run();
                }
                */
            }
        }
    }

    private void write_data() {

        try {
            OutputStreamWriter outputStreamWriter = new OutputStreamWriter(getApplicationContext().openFileOutput(filename, Context.MODE_PRIVATE));

            outputStreamWriter.write(move_mode);
            outputStreamWriter.write(p0_level);
            outputStreamWriter.write(p1_level);
            //outputStreamWriter.write(enable_hints?1:0);

            if (data_destroyed != 0) {
                num_of_redo = 0;
                num_of_history = 0;
                index_of_history = 0;
            }

            outputStreamWriter.write(num_of_redo);
            outputStreamWriter.write(num_of_history);
            outputStreamWriter.write(index_of_history);

            int i;
            int j = index_of_history - num_of_history;
            if (j < 0) {
                j = j + max_of_history;
            }
            for (i = 0; i < num_of_history + num_of_redo; i++) {
                outputStreamWriter.write(history[j].from_x);
                outputStreamWriter.write(history[j].from_y);
                outputStreamWriter.write(history[j].from_piece);
                outputStreamWriter.write(history[j].to_x);
                outputStreamWriter.write(history[j].to_y);
                outputStreamWriter.write(history[j].to_piece);
                j = j + 1;
                if (j == max_of_history) {
                    j = 0;
                }
            }

            outputStreamWriter.close();
        } catch (IOException e) {
            //Log.e("Exception", "File write failed: " + e.toString());
        }
    }

    private Boolean read_data() {
        //return false;


        try {
            InputStream inputStream = getApplicationContext().openFileInput(filename);

            if ( inputStream != null ) {
                InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
                move_mode = inputStreamReader.read();
                p0_level = inputStreamReader.read();
                p1_level = inputStreamReader.read();
                //enable_hints = (inputStreamReader.read() == 1);

                num_of_redo = inputStreamReader.read();
                num_of_history = inputStreamReader.read();
                index_of_history = inputStreamReader.read();

                if (num_of_redo > max_of_history - 1 || num_of_history > max_of_history ||
                        index_of_history > max_of_history - 1)  {
                    inputStream.close();
                    num_of_redo = 0;
                    num_of_history = 0;
                    index_of_history = 0;
                    return false;
                }

                int i;
                int j = index_of_history - num_of_history;
                if (j < 0) {
                    j = j + max_of_history;
                }
                for (i = 0; i < num_of_history + num_of_redo; i++) {

                    history[j].from_x = inputStreamReader.read();
                    history[j].from_y = inputStreamReader.read();
                    history[j].from_piece = inputStreamReader.read();
                    if ((char)history[j].from_piece == (char)-1) {
                        history[j].from_piece = -1;
                    }

                    if (history[j].from_x > 8 || history[j].from_y > 9 || history[j].from_piece > 31) {
                        inputStream.close();
                        num_of_redo = 0;
                        num_of_history = 0;
                        index_of_history = 0;
                        return false;
                    }

                    history[j].to_x = inputStreamReader.read();
                    history[j].to_y = inputStreamReader.read();
                    history[j].to_piece = inputStreamReader.read();
                    if ((char)history[j].to_piece == (char)-1) {
                        history[j].to_piece = -1;
                    }

                    if (history[j].to_x > 8 || history[j].to_y > 9 || history[j].to_piece > 31) {
                        inputStream.close();
                        num_of_redo = 0;
                        num_of_history = 0;
                        index_of_history = 0;
                        return false;
                    }

                    j = j + 1;
                    if (j == max_of_history) {
                        j = 0;
                    }
                }

                inputStream.close();
                return true;
            }
        }
        catch (FileNotFoundException e) {
            //Log.e(TAG_VALUE, "File not found: " + e.toString());
            return false;
        } catch (IOException e) {
            //Log.e(TAG_VALUE, "Can not read file: " + e.toString());
            return false;
        }
        return false;

    }

    /*
    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.d(TAG_VALUE, "onDestroy");
    }
     */

    @Override
    protected void onStop() {
        super.onStop();
        //Log.d(TAG_VALUE, "onStop");

        // Cancel the task
        if (computer_is_running) {
            m_task.cancel(true);
            computer_is_running = false;
        }

        write_data();
    }

    /*
    @Override
    protected void onPostResume() {
        super.onPostResume();
        Log.d(TAG_VALUE, "onResume");
    }
     */

    /*
    @Override
    protected void onRestart() {
        super.onRestart();
        Log.d(TAG_VALUE, "onRestart");
    }
    */

    /*
    @Override
    protected void onPause() {
        super.onPause();
        Log.d(TAG_VALUE, "onPause");
    }
    */

    /*
    @Override
    public void onStart() {
        super.onStart();
        Log.d(TAG_VALUE, "onStart");
    }
    */
}
