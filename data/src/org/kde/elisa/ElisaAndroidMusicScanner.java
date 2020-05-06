// SPDX-FileCopyrightText: 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

package org.kde.elisa;

import android.content.Context;
import android.database.Cursor;
import android.content.Intent;
import android.provider.MediaStore;

/**
 * Created by mgallien on 02/05/17.
 */

public class ElisaAndroidMusicScanner {
    private static String[] tracksRequestedColumns = {
        MediaStore.Audio.Media._ID,
        MediaStore.Audio.Media.TITLE,
        MediaStore.Audio.Media.TRACK,
        MediaStore.Audio.Media.YEAR,
        MediaStore.Audio.Media.DURATION,
        MediaStore.Audio.Media.DATA,
        MediaStore.Audio.Media.ARTIST,
        MediaStore.Audio.Media.ARTIST_ID,
        MediaStore.Audio.Media.ALBUM,
        MediaStore.Audio.Media.ALBUM_ID,
        MediaStore.Audio.Media.COMPOSER,
    };

    private static String[] albumsRequestedColumns = {
        MediaStore.Audio.Albums._ID,
        MediaStore.Audio.Albums.ALBUM,
        MediaStore.Audio.Albums.ALBUM_ART,
        MediaStore.Audio.Albums.ARTIST,
        MediaStore.Audio.Albums.NUMBER_OF_SONGS,
    };

    public static void listAudioFiles(Context ctx)
    {
        androidMusicScanTracksStarting();

        //Some audio may be explicitly marked as not being music
        String tracksSelection = MediaStore.Audio.Media.IS_MUSIC + " != 0";
        String tracksSortOrder = MediaStore.Audio.Media.DEFAULT_SORT_ORDER + " ASC";

        Cursor tracksCursor = ctx.getContentResolver().query(MediaStore.Audio.Media.EXTERNAL_CONTENT_URI, tracksRequestedColumns, tracksSelection, null, tracksSortOrder);

        tracksCursor.moveToFirst();
        
        while(tracksCursor.moveToNext()) {
            sendMusicFile(tracksCursor.getString(0) + "||" + tracksCursor.getString(1) + "||" +
                tracksCursor.getString(2) + "||" + tracksCursor.getString(3) + "||" + 
                tracksCursor.getString(4) + "||" + tracksCursor.getString(5) + "||" + 
                tracksCursor.getString(6) + "||" + tracksCursor.getString(7) + "||" + 
                tracksCursor.getString(8) + "||" + tracksCursor.getString(9) + "||" + 
                tracksCursor.getString(10));
        }

        androidMusicScanTracksFinishing();

        androidMusicScanAlbumsStarting();

        //Some audio may be explicitly marked as not being music
        String albumsSortOrder = MediaStore.Audio.Albums.DEFAULT_SORT_ORDER + " ASC";

        Cursor albumsCursor = ctx.getContentResolver().query(MediaStore.Audio.Albums.EXTERNAL_CONTENT_URI, albumsRequestedColumns, null, null, albumsSortOrder);

        albumsCursor.moveToFirst();
        
        while(albumsCursor.moveToNext()) {
            sendMusicAlbum(albumsCursor.getString(0) + "||" + albumsCursor.getString(1) + "||" + 
            albumsCursor.getString(2) + "||" + albumsCursor.getString(3) + "||" + albumsCursor.getString(4));
        }
        
        androidMusicScanAlbumsFinishing();
    }

    private static native void androidMusicScanTracksStarting();

    private static native void sendMusicFile(String musicFile);

    private static native void androidMusicScanTracksFinishing();

    private static native void androidMusicScanAlbumsStarting();

    private static native void sendMusicAlbum(String musicFile);

    private static native void androidMusicScanAlbumsFinishing();

}
