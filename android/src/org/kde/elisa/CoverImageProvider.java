// SPDX-FileCopyrightText: 2025 (c) Jack Hill <jackhill3103@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

package org.kde.elisa;

import android.annotation.SuppressLint;
import android.content.ContentUris;
import android.content.Context;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Build;
import android.provider.MediaStore;
import android.util.Log;
import android.util.Size;

@SuppressLint("LongLogTag")
public final class CoverImageProvider
{
    private static final String TAG = "org.kde.elisa.CoverImageProvider";

    public static Bitmap contentThumbnail(Context context, String contentUri, int width, int height)
    {
        Log.d(TAG, "contentThumbnail for " + contentUri);
        try {
            Size x = new Size(width > 0 ? width : 50, height > 0 ? height : 50);

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
                return context.getContentResolver().loadThumbnail(
                        Uri.parse(contentUri.replaceFirst("^image://android/", "")), x, null);
            } else {
                Uri sArtworkUri = Uri.parse("content://media/external/audio/albumart");
                Uri uri = ContentUris.withAppendedId(sArtworkUri, ContentUris.parseId(Uri.parse(contentUri)));

                return MediaStore.Images.Media.getBitmap(context.getContentResolver(), uri);
            }
        } catch (Exception e) {
            Log.d(TAG, "contentThumbnail failed to load bitmap for " + contentUri, e);
            return Bitmap.createBitmap(1, 1, Bitmap.Config.ARGB_8888);
        }
    }
}