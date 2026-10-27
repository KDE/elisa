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

    public static final String PLAYBACK_STATE_PROPERTY = "PlaybackState";
    public static final String POSITION_PROPERTY = "Position";
    public static final String METADATA_PROPERTY = "Metadata";

    private static volatile QtSignalListener<Boolean> propertyChangedListener;

    public static volatile PlaybackState playbackState = PlaybackState.StoppedState;
    public static volatile long position = 0;
    public static volatile Metadata metadata = new Metadata();

    public static native void next();
    public static native void previous();
    public static native void playPause();
    public static native void play();
    public static native void pause();
    public static native void stop();
    public static native void seek(long offset);

    public static void propertyChanged(String property, Object value)
    {
        if (!POSITION_PROPERTY.equals(property)) {
            Log.d(TAG, "propertyChanged: " + property + " - " + value);
        }
        boolean shouldUpdate = true;

        try {
            switch (property) {
                case PLAYBACK_STATE_PROPERTY:
                    playbackState = PlaybackState.values[(int) value];
                    break;
                case POSITION_PROPERTY:
                    position = (long) value;
                    break;
                case METADATA_PROPERTY:
                    metadata = (Metadata) value;
                    position = 0;
                    break;
                default:
                    shouldUpdate = false;
                    break;
            }
        } catch (Exception e) {
            Log.e(TAG, "propertyChanged: cannot apply " + property + " = " + value, e);
            shouldUpdate = false;
        }

        final QtSignalListener<Boolean> listener = propertyChangedListener;
        if (listener != null) {
            listener.onSignalEmitted(property, shouldUpdate);
        }
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
