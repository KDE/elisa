// SPDX-FileCopyrightText: 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

package org.kde.elisa;

import android.annotation.SuppressLint;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ServiceInfo;
import android.graphics.Bitmap;
import android.os.IBinder;
import android.os.Build;
import android.util.Log;

import android.support.v4.media.MediaMetadataCompat;
import android.support.v4.media.session.MediaSessionCompat;
import android.support.v4.media.session.PlaybackStateCompat;

import androidx.media.app.NotificationCompat.MediaStyle;
import androidx.core.app.PendingIntentCompat;
import androidx.core.app.ServiceCompat;
import androidx.core.app.NotificationCompat;
import androidx.media.session.MediaButtonReceiver;
import androidx.palette.graphics.Palette;

import org.qtproject.qt.android.QtSignalListener;

@SuppressLint("LongLogTag")
public class ElisaService extends Service implements QtSignalListener<Boolean> {
    private static final String TAG = "org.kde.elisa.ElisaService";
    private static final String CHANNEL_ID = "background_playback";
    private static final int NOTIFICATION_ID = 0xE195A;

    private MediaSessionCompat mediaSession = null;
    final private MediaMetadataCompat.Builder mediaMetadataBuilder = new MediaMetadataCompat.Builder();
    final private PlaybackStateCompat.Builder playbackStateBuilder = new PlaybackStateCompat.Builder();

    private final MediaSessionCompat.Callback mediaSessionCallback = new MediaSessionCompat.Callback()
    {
        @Override
        public void onPause() {
            if (Player.playbackState == Player.PlaybackState.PlayingState) {
                Player.playPause();
            }
        }

        @Override
        public void onPlay() {
            if (Player.playbackState == Player.PlaybackState.StoppedState) {
                Player.playPause();
            }
        }

        @Override
        public void onSeekTo(long pos) {
            Player.seek(pos);
        }

        @Override
        public void onSkipToNext() {
            Player.next();
        }

        @Override
        public void onSkipToPrevious() {
            Player.previous();
        }

        @Override
        public void onStop() {
            Player.stop();
        }
    };

    @Override
    public void onCreate()
    {
        super.onCreate();

        Player.setPropertyChangedListener(this);

        mediaSession = new MediaSessionCompat(this, TAG);
        mediaSession.setCallback(mediaSessionCallback);
        updateMediaSession();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        ComponentName componentName = intent.getComponent();
        if (componentName != null && componentName.getClassName().equals(this.getClass().getName())) {
            refreshNotification();
        }

        MediaButtonReceiver.handleIntent(mediaSession, intent);
        return START_NOT_STICKY;
    }

    @Override
    public void onDestroy()
    {
        ServiceCompat.stopForeground(this, ServiceCompat.STOP_FOREGROUND_REMOVE);
        Player.setPropertyChangedListener(null);

        if (mediaSession != null) {
            mediaSession.setActive(false);
            mediaSession.release();
        }
        mediaSession = null;

        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onSignalEmitted(String property, Boolean shouldUpdate)
    {
        Log.d(TAG, "onSignalEmitted: " + property + " - shouldUpdate:" + shouldUpdate);
        if (shouldUpdate) {
            updateMediaSession();
            refreshNotification();
        }
    }

    private void refreshNotification()
    {
        ServiceCompat.startForeground(this, NOTIFICATION_ID, buildNotification(),
                Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q ? ServiceInfo.FOREGROUND_SERVICE_TYPE_MEDIA_PLAYBACK: 0);
    }

    private Notification buildNotification()
    {
        NotificationCompat.Builder notificationBuilder = new NotificationCompat.Builder(this, CHANNEL_ID);

        if (Player.playbackState == Player.PlaybackState.StoppedState)
            return notificationBuilder.build();

        Intent notificationIntent = new Intent(this, ElisaActivity.class);
        PendingIntent pendingIntent = PendingIntentCompat.getActivity(this, 0, notificationIntent, 0, false);

        notificationBuilder
                .setVisibility(NotificationCompat.VISIBILITY_PUBLIC)
                .setPriority(NotificationCompat.PRIORITY_DEFAULT)
                .setSmallIcon(R.mipmap.ic_notification_elisa)
                .setContentTitle(!Player.metadata.title.isEmpty() ? Player.metadata.title : "Elisa")
                .setContentText(Player.metadata.artist)
                .setContentIntent(pendingIntent)
                .setOngoing(true)
                .addAction(R.drawable.ic_previous, "Previous",
                        MediaButtonReceiver.buildMediaButtonPendingIntent(this, PlaybackStateCompat.ACTION_SKIP_TO_PREVIOUS));

        if (Player.playbackState == Player.PlaybackState.PlayingState) {
            notificationBuilder.addAction(R.drawable.ic_pause, "Pause",
                    MediaButtonReceiver.buildMediaButtonPendingIntent(this, PlaybackStateCompat.ACTION_PAUSE));
        } else {
            notificationBuilder.addAction(R.drawable.ic_play, "Play",
                    MediaButtonReceiver.buildMediaButtonPendingIntent(this, PlaybackStateCompat.ACTION_PLAY));
        }

        notificationBuilder.addAction(R.drawable.ic_next, "Next",
                MediaButtonReceiver.buildMediaButtonPendingIntent(this, PlaybackStateCompat.ACTION_SKIP_TO_NEXT));

        if (!Player.metadata.albumCover.isEmpty()) {
            Bitmap cover = CoverImageProvider.contentThumbnail(this, Player.metadata.albumCover, 512, 512);

            if (cover != null) {
                notificationBuilder.setLargeIcon(cover);

                notificationBuilder.setColorized(true);
                notificationBuilder.setColor(Palette.from(cover).generate().getVibrantColor(0));
            }
        }

        MediaStyle style = new MediaStyle();
        style.setShowActionsInCompactView(0, 1, 2);
        style.setMediaSession(mediaSession.getSessionToken());
        notificationBuilder.setStyle(style);

        return notificationBuilder.build();
    }
    private MediaMetadataCompat buildMediaMetadata()
    {
        if (Player.playbackState == Player.PlaybackState.StoppedState)
            return null;

        if (!Player.metadata.albumCover.isEmpty()) {
            mediaMetadataBuilder.putBitmap(MediaMetadataCompat.METADATA_KEY_ART, CoverImageProvider.contentThumbnail(
                    this, Player.metadata.albumCover, 512, 512));
        } else {
            mediaMetadataBuilder.putBitmap(MediaMetadataCompat.METADATA_KEY_ART, null);
        }

        return mediaMetadataBuilder
                .putText(MediaMetadataCompat.METADATA_KEY_ALBUM, Player.metadata.albumName)
                .putText(MediaMetadataCompat.METADATA_KEY_ARTIST, Player.metadata.artist)
                .putLong(MediaMetadataCompat.METADATA_KEY_DURATION, Player.metadata.duration)
                .putText(MediaMetadataCompat.METADATA_KEY_TITLE, Player.metadata.title)
                .build();
    }

    private PlaybackStateCompat buildPlaybackState()
    {
        if (Player.playbackState == Player.PlaybackState.StoppedState)
            return null;

        int state = PlaybackStateCompat.STATE_NONE;
        switch (Player.playbackState) {
            case PlayingState:
                state = PlaybackStateCompat.STATE_PLAYING;
                break;
            case PausedState:
                state = PlaybackStateCompat.STATE_PAUSED;
                break;
        }

        return playbackStateBuilder
                .setState(state, Player.position, 1f)
                .setActions(
                        PlaybackStateCompat.ACTION_PLAY |
                        PlaybackStateCompat.ACTION_PLAY_PAUSE |
                        PlaybackStateCompat.ACTION_PAUSE |
                        PlaybackStateCompat.ACTION_SKIP_TO_PREVIOUS |
                        PlaybackStateCompat.ACTION_SKIP_TO_NEXT |
                        PlaybackStateCompat.ACTION_SEEK_TO)
                .build();
    }

    private void updateMediaSession()
    {
        PlaybackStateCompat state = buildPlaybackState();
        mediaSession.setMetadata(buildMediaMetadata());
        mediaSession.setActive(Player.playbackState != Player.PlaybackState.StoppedState);
        mediaSession.setPlaybackState(playbackStateBuilder.build());
    }

    public static void createNotificationChannel(Context context)
    {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(
                    CHANNEL_ID,
                    "Elisa Background Playback",
                    NotificationManager.IMPORTANCE_MIN
            );

            NotificationManager notificationManager = context.getSystemService(NotificationManager.class);
            notificationManager.createNotificationChannel(channel);
        }
    }
}
