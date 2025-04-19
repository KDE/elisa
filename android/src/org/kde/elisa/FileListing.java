// SPDX-FileCopyrightText: 2025 Matthieu Gallien <matthieu_gallien@yahoo.fr>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

package org.kde.elisa;

import android.annotation.SuppressLint;
import android.content.ContentUris;
import android.content.Context;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.Manifest;
import android.provider.MediaStore;
import android.util.Log;
import androidx.core.content.ContextCompat;

import java.util.ArrayList;

@SuppressLint("LongLogTag")
public final class FileListing
{
    private static final String TAG = "org.kde.elisa.FileListing";

    final private static String[] tracksRequestedColumns = {
            MediaStore.Audio.Media._ID,
            MediaStore.Audio.Media.TITLE,
            MediaStore.Audio.Media.ARTIST,
            MediaStore.Audio.Media.ARTIST_ID,
            MediaStore.Audio.Media.ALBUM,
            MediaStore.Audio.Media.ALBUM_ID,
            MediaStore.Audio.Media.ALBUM_ARTIST,
            MediaStore.Audio.Media.TRACK,
            MediaStore.Audio.Media.DURATION,
            MediaStore.Audio.Media.DATA,
            MediaStore.Audio.Media.COMPOSER,
    };

    public static ArrayList<Object> listAudioFiles(Context ctx)
    {
        Log.d(TAG, "listAudioFiles");

        ArrayList<Object> allTracks = new ArrayList<>();

        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.TIRAMISU) {
            if (ContextCompat.checkSelfPermission(ctx, Manifest.permission.READ_MEDIA_AUDIO)
                    != PackageManager.PERMISSION_GRANTED) {
                Log.d(TAG, "NO READ_MEDIA_AUDIO");
                return allTracks;
            }
        } else {
            if (ContextCompat.checkSelfPermission(ctx, Manifest.permission.READ_EXTERNAL_STORAGE)
                    != PackageManager.PERMISSION_GRANTED) {
                Log.d(TAG, "NO READ_EXTERNAL_STORAGE");
                return allTracks;
            }
        }

        String tracksSelection = MediaStore.Audio.Media.IS_MUSIC + " != 0";
        String tracksSortOrder = MediaStore.Audio.Media.DEFAULT_SORT_ORDER + " ASC";

        try (Cursor tracksCursor = ctx.getContentResolver().query(MediaStore.Audio.Media.EXTERNAL_CONTENT_URI, tracksRequestedColumns, tracksSelection, null, tracksSortOrder)) {

            if (tracksCursor == null || !tracksCursor.moveToFirst()) {
                Log.d(TAG, "Tracks cursor empty");
                return allTracks;
            }

            do {
                String albumArtUri = ContentUris.withAppendedId(MediaStore.Audio.Albums.EXTERNAL_CONTENT_URI, tracksCursor.getLong(5)).toString();

                int trackAndDiscNumber = tracksCursor.getInt(7);

                allTracks.add(new TrackDataType(tracksCursor.getString(1), tracksCursor.getString(2),
                        tracksCursor.getString(4), tracksCursor.getString(6),
                        trackAndDiscNumber % 1000, trackAndDiscNumber / 1000,
                        tracksCursor.getLong(8), tracksCursor.getString(9),
                        albumArtUri, "",
                        tracksCursor.getString(10)));
            } while (tracksCursor.moveToNext());
        }

        return allTracks;
    }
}

class TrackDataType
{
    TrackDataType (String title, String artist, String albumName,
                   String albumArtist, int trackNumber, int discNumber,
                   long duration, String resourceURI, String albumCover,
                   String genre, String composer) {
        this.mTitle = title;
        this.mArtist = artist;
        this.mAlbumName = albumName;
        this.mAlbumArtist = albumArtist;
        this.mTrackNumber = trackNumber;
        this.mDiscNumber = discNumber;
        this.mDuration = duration;
        this.mResourceURI = resourceURI;
        this.mAlbumCover = albumCover;
        this.mGenre = genre;
        this.mComposer = composer;
    }

    public String getTitle() {
        return mTitle;
    }

    public String getArtist() {
        return mArtist;
    }

    public String getAlbumName() {
        return mAlbumName;
    }

    public String getAlbumArtist() {
        return mAlbumArtist;
    }

    public int getTrackNumber() {
        return mTrackNumber;
    }

    public int getDiscNumber() {
        return mDiscNumber;
    }

    public long getDuration() {
        return mDuration;
    }

    public String getResourceURI() {
        return mResourceURI;
    }

    public String getAlbumCover() {
        return mAlbumCover;
    }

    public String getGenre() {
        return mGenre;
    }

    public String getComposer() {
        return mComposer;
    }

    final private String mTitle;
    final private String mArtist;
    final private String mAlbumName;
    final private String mAlbumArtist;
    final private int mTrackNumber;
    final private int mDiscNumber;
    final private long mDuration;
    final private String mResourceURI;
    final private String mAlbumCover;
    final private String mGenre;
    final private String mComposer;
}
