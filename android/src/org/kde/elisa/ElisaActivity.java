// SPDX-FileCopyrightText: 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

package org.kde.elisa;

import org.qtproject.qt.android.bindings.QtActivity;
import android.content.Context;
import android.graphics.Bitmap;
import android.net.Uri;
import android.util.Log;
import android.util.Size;

public class ElisaActivity extends QtActivity
{
    private static final String TAG = "org.kde.elisa.android.java ElisaActivity";

    public static Bitmap contentThumbnail(Context context, String contentUri, int width, int height)
    {
        try {
            Size x = new Size(width > 0 ? width : 50, height > 0 ? height : 50);
            Bitmap bitmap = context.getContentResolver().loadThumbnail(Uri.parse(contentUri), x, null);
            return bitmap;
        } catch (Exception e) {
            Log.d(TAG, "contentThumbnail failed to load bitmap for " + contentUri, e);
            Bitmap.Config config = Bitmap.Config.ARGB_8888;
            return Bitmap.createBitmap(0, 0, config);
        }
    }
}