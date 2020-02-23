/*
Main2Activity.java - Copyright (c) 2020 IcebergFu's Software, www.icebergfu.com

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

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.RadioGroup;

public class Main2Activity extends AppCompatActivity {

    public static final String EXTRA_REPLY =
            "com.icebergfu.chinesechess.main2activity.extra.REPLY";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);
        Intent intent = getIntent();
        String message = intent.getStringExtra(EXTRA_REPLY);
        int option = Integer.parseInt(message);
        int p0 = option & 7;
        int p1 = (option >> 3) & 7;
        RadioGroup radioGroup0 = findViewById(R.id.p0);
        if (p0 == 0) {
            radioGroup0.check(R.id.p0first);
        } else if (p0 == 1) {
            radioGroup0.check(R.id.p0second);
        } else if (p0 == 2) {
            radioGroup0.check(R.id.p0third);
        } else if (p0 == 3) {
            radioGroup0.check(R.id.p0fourth);
        } /* else if (p0 == 4) {
            radioGroup0.check(R.id.p0fifth);
        } */

        RadioGroup radioGroup1 = findViewById(R.id.p1);
        if (p1 == 0) {
            radioGroup1.check(R.id.p1first);
        } else if (p1 == 1) {
            radioGroup1.check(R.id.p1second);
        } else if (p1 == 2) {
            radioGroup1.check(R.id.p1third);
        } else if (p1 == 3) {
            radioGroup1.check(R.id.p1fourth);
        } /* else if (p1 == 4) {
            radioGroup1.check(R.id.p1fifth);
        } */
    }

    public void returnReply(View view) {
        RadioGroup radioGroup0 = findViewById(R.id.p0);
        int checkedRadioButtonId = radioGroup0.getCheckedRadioButtonId();
        int p0 = 0;
        /* if (checkedRadioButtonId == R.id.p0first) {
            p0 = 0;
        } else */ if (checkedRadioButtonId == R.id.p0second) {
            p0 = 1;
        } else if (checkedRadioButtonId == R.id.p0third) {
            p0 = 2;
        } else if (checkedRadioButtonId == R.id.p0fourth) {
            p0 = 3;
        } /* else if (checkedRadioButtonId == R.id.p0fifth) {
            p0 = 4;
        } */

        RadioGroup radioGroup1 = findViewById(R.id.p1);
        int checkedRadioButtonId1 = radioGroup1.getCheckedRadioButtonId();
        int p1 = 0;
        /* if (checkedRadioButtonId1 == R.id.p1first) {
            p1 = 0;
        } else */ if (checkedRadioButtonId1 == R.id.p1second) {
            p1 = 1;
        } else if (checkedRadioButtonId1 == R.id.p1third) {
            p1 = 2;
        } else if (checkedRadioButtonId1 == R.id.p1fourth) {
            p1 = 3;
        } /* else if (checkedRadioButtonId1 == R.id.p1fifth) {
            p1 = 4;
        } */

        int option = p0 + (p1 << 3);
        String reply = Integer.toString(option);
        Intent replyIntent = new Intent();
        replyIntent.putExtra(EXTRA_REPLY, reply);
        setResult(RESULT_OK, replyIntent);
        finish();
    }
}
