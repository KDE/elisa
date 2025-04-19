// SPDX-FileCopyrightText: 2025 (c) Pedro Nishiyama <nishiyama.v3@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

package org.kde.elisa;

import android.annotation.SuppressLint;
import android.util.Log;

import org.qtproject.qt.android.QtSignalListener;

@SuppressLint("LongLogTag")
public final class Player
{
    private static final String TAG = "org.kde.elisa.Player";

    private static QtSignalListener<Boolean> propertyChangedListener;

    public static PlaybackState playbackState = PlaybackState.StoppedState;
    public static long position = 0;
    public static Metadata metadata = new Metadata();

    public static native void next();
    public static native void previous();
    public static native void playPause();
    public static native void stop();
    public static native void seek(long offset);


    public static void propertyChanged(String property, Object value)
    {
        Log.d(TAG, "propertyChanged: " + property + " - " + value);
        boolean shouldUpdate = true;

        try {
            switch (property) {
                case "PlaybackState":
                    playbackState = PlaybackState.values[(int) value];
                    break;
                case "Position":
                    position = (long) value;
                    break;
                case "Metadata":
                    metadata = (Metadata) value;
                    position = 0;
                    break;
                default:
                    shouldUpdate = false;
                    break;
            }
        } catch (Exception e) {
            shouldUpdate = false;
        }

        propertyChangedListener.onSignalEmitted("propertyChanged", shouldUpdate);
    }

    public static void setPropertyChangedListener(QtSignalListener<Boolean> listener)
    {
        propertyChangedListener = listener;
    }

    public enum PlaybackState {
        StoppedState,
        PlayingState,
        PausedState;

        final static PlaybackState[] values = PlaybackState.values();
    }
}
