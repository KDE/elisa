// SPDX-FileCopyrightText: 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
// SPDX-FileCopyrightText: 2026 Ian Monroe <imonroe@kde.org>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

package org.kde.elisa;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;

import org.libsdl.app.SDL;
import org.qtproject.qt.android.bindings.QtActivity;

@SuppressLint("LongLogTag")
public class ElisaActivity extends QtActivity
{
    private static final String TAG = "org.kde.elisa.ElisaActivity";

    private static void initSdlForActivity(ElisaActivity activity)
    {
        try {
            if (Build.SUPPORTED_ABIS != null && Build.SUPPORTED_ABIS.length > 0) {
                try {
                    System.loadLibrary("SDL3_" + Build.SUPPORTED_ABIS[0]);
                } catch (UnsatisfiedLinkError ignored) {
                    System.loadLibrary("SDL3");
                }
            } else {
                System.loadLibrary("SDL3");
            }

            SDL.setupJNI();
            SDL.initialize();
            SDL.setContext(activity);
        } catch (UnsatisfiedLinkError e) {
            Log.w(TAG, "SDL3 native library not present; skipping SDL JNI setup: " + e.getMessage());
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        initSdlForActivity(this);

        super.onCreate(savedInstanceState);

        ElisaService.createNotificationChannel(this);
        startService(new Intent(this, ElisaService.class));
    }

    @Override
    protected void onDestroy()
    {
        stopService(new Intent(this, ElisaService.class));

        super.onDestroy();
    }
}
