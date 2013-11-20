package com.leighpauls.mouseypebb;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;

import java.util.UUID;

public class MouseyPebb extends Activity {

    PebbleKit.PebbleDataReceiver mDataReceiver;

    enum Keys {
        ACCEL_X_POS(1),
        ACCEL_Y_POS(2),
        LEFT_MOUSE_STATE(10),
        MIDDLE_MOUSE_STATE(11),
        RIGHT_MOUSE_STATE(12);

        public final int value;
        private Keys(int value) {
            this.value = value;
        }
    }

    /**
     * Called when the activity is first created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        mDataReceiver = new PebbleKit.PebbleDataReceiver(
                UUID.fromString("b91b0abd-abab-48e5-8f94-5e6d8da59c92")) {
            @Override
            public void receiveData(Context context, int transactionId, PebbleDictionary data) {
                if (data.contains(Keys.ACCEL_X_POS.value)) {
                    // Log.i("LEIGHPAULS", "X position: " + data.getInteger(Keys.ACCEL_X_POS.value));
                }
                if (data.contains(Keys.ACCEL_Y_POS.value)) {
                    // Log.i("LEIGHPAULS", "Y position: " + data.getInteger(Keys.ACCEL_Y_POS.value));
                }
                if (data.contains(Keys.LEFT_MOUSE_STATE.value)) {
                    // Log.i("LEIGHPAULS", "LEFT MOUSE: " + data.getInteger(Keys.LEFT_MOUSE_STATE.value));
                }


                PebbleKit.sendAckToPebble(context, transactionId);
            }
        };
        PebbleKit.registerReceivedDataHandler(this, mDataReceiver);
    }

    @Override
    public void onDestroy() {
        unregisterReceiver(mDataReceiver);
    }
}
