// SPDX-FileCopyrightText: 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

package org.kde.elisa;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

public class ElisaService extends Service {
    private static final String TAG = "org.kde.elisa.ElisaService";

    @Override
    public void onCreate() {
        super.onCreate();
        // TODO: Implement MediaSession... Soon TM
        Log.d(TAG, "onCreate");
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}
