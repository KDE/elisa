// SPDX-FileCopyrightText: 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

package org.kde.elisa;

import org.qtproject.qt5.android.bindings.QtActivity;
import android.content.ContentResolver;
import android.content.Context;
import androidx.core.content.ContextCompat;
import androidx.core.app.ActivityCompat;
import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.content.Intent;
import android.provider.MediaStore;
import java.util.ArrayList;
import java.util.HashMap;

public class ElisaActivity extends QtActivity
{
    private static String[] tracksRequestedColumns = {
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

    private static String[] albumsRequestedColumns = {
        MediaStore.Audio.Albums._ID,
        MediaStore.Audio.Albums.ALBUM,
        MediaStore.Audio.Albums.ARTIST,
        MediaStore.Audio.Albums.ALBUM_ART,
    };

    public static ArrayList<TrackDataType> listAudioFiles(Context ctx)
    {
        ArrayList<TrackDataType> allTracks = new ArrayList<TrackDataType>();

        HashMap<Integer, AlbumDataType> allAlbums = new HashMap<Integer, AlbumDataType>();

        if (ContextCompat.checkSelfPermission(ctx, Manifest.permission.READ_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            return allTracks;
        }

        Cursor albumsCursor = ctx.getContentResolver().query(MediaStore.Audio.Albums.EXTERNAL_CONTENT_URI, albumsRequestedColumns, null, null, null);

        albumsCursor.moveToFirst();

        if (albumsCursor == null) {
            return allTracks;
        }

        if (!albumsCursor.moveToFirst()) {
            return allTracks;
        }

        do {
            allAlbums.put(albumsCursor.getInt(0), new AlbumDataType(albumsCursor.getString(1), albumsCursor.getString(2), albumsCursor.getString(3)));
        } while(albumsCursor.moveToNext());

        String tracksSelection = MediaStore.Audio.Media.IS_MUSIC + " != 0";
        String tracksSortOrder = MediaStore.Audio.Media.DEFAULT_SORT_ORDER + " ASC";

        Cursor tracksCursor = ctx.getContentResolver().query(MediaStore.Audio.Media.EXTERNAL_CONTENT_URI, tracksRequestedColumns, tracksSelection, null, tracksSortOrder);

        if (tracksCursor == null) {
            return allTracks;
        }

        if (!tracksCursor.moveToFirst()) {
            return allTracks;
        }

        do {
            AlbumDataType currentAlbum = allAlbums.get(tracksCursor.getInt(5));

            int trackAndDiscNumber = tracksCursor.getInt(7);

            allTracks.add(new TrackDataType(tracksCursor.getString(1), tracksCursor.getString(2),
                                          tracksCursor.getString(4), tracksCursor.getString(6),
                                          trackAndDiscNumber % 1000, trackAndDiscNumber / 1000,
                                          tracksCursor.getLong(8), tracksCursor.getString(9),
                                          currentAlbum.getAlbumCover(), "",
                                          tracksCursor.getString(10)));
        } while(tracksCursor.moveToNext());

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

    private String mTitle;
    private String mArtist;
    private String mAlbumName;
    private String mAlbumArtist;
    private int mTrackNumber;
    private int mDiscNumber;
    private long mDuration;
    private String mResourceURI;
    private String mAlbumCover;
    private String mGenre;
    private String mComposer;
}

class AlbumDataType
{
    AlbumDataType (String albumName, String albumArtist, String albumCover) {
       this.mAlbumName = albumName;
       this.mAlbumArtist = albumArtist;
       this.mAlbumCover = albumCover;
    }

    public String getAlbumName() {
        return mAlbumName;
    }

    public String getAlbumArtist() {
        return mAlbumArtist;
    }

    public String getAlbumCover() {
        return mAlbumCover;
    }

    private String mAlbumName;
    private String mAlbumArtist;
    private String mAlbumCover;
}
