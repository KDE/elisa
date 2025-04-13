// SPDX-FileCopyrightText: 2025 (c) Jack Hill <jackhill3103@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

package org.kde.elisa;

import android.content.Context;
import android.graphics.Bitmap;
import android.net.Uri;
import android.util.Log;
import android.util.Size;

public final class CoverImageProvider
{
    private static final String TAG = "org.kde.elisa.CoverImageProvider";

    public static Bitmap contentThumbnail(Context context, String contentUri, int width, int height)
    {
        Log.d(TAG, "contentThumbnail for " + contentUri);
        try {
            Size x = new Size(width > 0 ? width : 50, height > 0 ? height : 50);
            return context.getContentResolver().loadThumbnail(Uri.parse(contentUri), x, null);
        } catch (Exception e) {
            Log.d(TAG, "contentThumbnail failed to load bitmap for " + contentUri, e);
            return Bitmap.createBitmap(1, 1, Bitmap.Config.ARGB_8888);
        }
    }
}