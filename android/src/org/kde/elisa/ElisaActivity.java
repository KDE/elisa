// SPDX-FileCopyrightText: 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

package org.kde.elisa;

import android.os.Bundle;
import android.util.Log;

import org.qtproject.qt.android.bindings.QtActivity;

public class ElisaActivity extends QtActivity
{
    private static final String TAG = "org.kde.elisa.ElisaActivity";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(TAG, "onCreate: "+savedInstanceState);

        // TODO: Implement MediaSession
    }
}