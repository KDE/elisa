// SPDX-FileCopyrightText: 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

package org.kde.elisa;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import androidx.core.content.ContextCompat;

import org.qtproject.qt.android.bindings.QtActivity;

@SuppressLint("LongLogTag")
public class ElisaActivity extends QtActivity
{
    private static final String TAG = "org.kde.elisa.ElisaActivity";

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        Log.d(TAG, "onCreate: "+savedInstanceState);

        ElisaService.createNotificationChannel(this);

        Intent serviceIntent = new Intent(this, ElisaService.class);
        ContextCompat.startForegroundService(this, serviceIntent);
    }

    @Override
    public void onDestroy()
    {
        Intent serviceIntent = new Intent(this, ElisaService.class);
        stopService(serviceIntent);

        super.onDestroy();
    }
}