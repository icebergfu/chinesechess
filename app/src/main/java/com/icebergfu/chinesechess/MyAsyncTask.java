/*
MyAsyncTask.java - Copyright (c) 2020 IcebergFu's Software, www.icebergfu.com

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

import android.os.AsyncTask;
//import android.view.Gravity;
//import android.widget.TextView;
//import android.widget.Toast;

import java.lang.ref.WeakReference;
//import java.util.Random;

public class MyAsyncTask extends AsyncTask <Void, Void, Integer> {
    private WeakReference<MainActivity> mMainActivity;
    //public native int computerMoveFromJNI();
    public native int computerMove0FromJNI();
    public native int computerMove1FromJNI();
    public native int computerMove2FromJNI(int depth);
    public native boolean addMoveFromJNI(int move);

    MyAsyncTask(MainActivity tv) {
        mMainActivity = new WeakReference<>(tv);
    }

    @Override
    protected Integer doInBackground(Void... voids) {
        //return computerMoveFromJNI();

        // Step 0.
        int move = computerMove0FromJNI();
        if (move != 0) {
            return move;
        }

        int i;
        int LIMIT_DEPTH = 64;
        while (true) {
            // Step 1.
            computerMove1FromJNI();

            for (i = 1; i <= LIMIT_DEPTH; i++) {
                if (isCancelled()) {
                    // Got cancelled, just return 0;
                    return 0;
                }

                // Step 2.
                int this_move = computerMove2FromJNI(i);
                if (this_move == 0) {
                    // End of search
                    break;
                }
                move = this_move;
                //publishProgress(i);
            }
            if (isCancelled()) {
                // Got cancelled, just return 0;
                return 0;
            }
            if (move != 0) {
                return move;
            }
        }
    }

    protected void onPostExecute(Integer result) {
        // We can only add move to JNI here.
        addMoveFromJNI(result);
        mMainActivity.get().let_computer_run2(result);
    }

    /*
    protected void onProgressUpdate(Integer... progress){
            mMainActivity.get().update_alarm(true);

    }
    */
}
