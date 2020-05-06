// SPDX-FileCopyrightText: 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

package org.kde.elisa;

import android.content.Context;
import android.database.Cursor;
import android.content.Intent;
import android.provider.MediaStore;

import org.qtproject.qt5.android.bindings.QtService;

class ElisaNatives {

    // declare the native method
    public static native void sendMusicFile(String musicFile);
}

/**
 * Created by mgallien on 02/05/17.
 */

public class ElisaService extends QtService {
    public static void startMyService(Context ctx) {
        //System.err.println("hello service");
        ctx.startService(new Intent(ctx, ElisaService.class));
    }
}
