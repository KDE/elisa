// SPDX-FileCopyrightText: 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
// SPDX-FileCopyrightText: 2026 Ian Monroe <imonroe@kde.org>
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
import android.os.Handler;
import android.os.IBinder;
import android.os.Build;
import android.os.Looper;
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
    private static final long IDLE_STOP_DELAY_MS = 10000L;
    private static final int COVER_SIZE = 512;

    private MediaSessionCompat mediaSession = null;
    private ElisaAudioFocusHandler audioFocusHandler = null;
    // True while the service was started by MediaButtonReceiver with
    // startForegroundService() and still owes the platform a startForeground()
    // call, even if playback never actually starts.
    private boolean pendingForegroundStart = false;
    final private MediaMetadataCompat.Builder mediaMetadataBuilder = new MediaMetadataCompat.Builder();
    final private PlaybackStateCompat.Builder playbackStateBuilder = new PlaybackStateCompat.Builder();

    private final Handler mainHandler = new Handler(Looper.getMainLooper());
    private final Runnable stopIfIdle = new Runnable() {
        @Override
        public void run() {
            if (Player.playbackState == Player.PlaybackState.StoppedState) {
                Log.d(TAG, "A media button did not start playback; stopping the service");
                ServiceCompat.stopForeground(ElisaService.this, ServiceCompat.STOP_FOREGROUND_REMOVE);
                stopSelf();
            }
        }
    };

    private String cachedCoverUrl = null;
    private Bitmap cachedCover = null;
    private int cachedCoverColor = 0;
    private boolean cachedCoverColorValid = false;

    private final MediaSessionCompat.Callback mediaSessionCallback = new MediaSessionCompat.Callback()
    {
        @Override
        public void onPause() {
            if (Player.playbackState == Player.PlaybackState.PlayingState) {
                Player.pause();
            }
        }

        @Override
        public void onPlay() {
            if (Player.playbackState != Player.PlaybackState.PlayingState
                    && (audioFocusHandler == null || audioFocusHandler.requestFocus())) {
                Player.play();
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

        audioFocusHandler = new ElisaAudioFocusHandler(this);
        Player.setPropertyChangedListener(this);

        mediaSession = new MediaSessionCompat(this, TAG);
        mediaSession.setCallback(mediaSessionCallback);
        updateMediaSession();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        if (intent != null) {
            if (Intent.ACTION_MEDIA_BUTTON.equals(intent.getAction())) { // for eg a button on your bluetooth headphones
                pendingForegroundStart = true;
                mainHandler.removeCallbacks(stopIfIdle);
                mainHandler.postDelayed(stopIfIdle, IDLE_STOP_DELAY_MS);
            }
            ComponentName componentName = intent.getComponent();
            if (componentName != null && componentName.getClassName().equals(this.getClass().getName())) {
                refreshNotification();
            }
            MediaButtonReceiver.handleIntent(mediaSession, intent);
        }
        return START_NOT_STICKY;
    }

    @Override
    public void onDestroy()
    {
        mainHandler.removeCallbacks(stopIfIdle);
        ServiceCompat.stopForeground(this, ServiceCompat.STOP_FOREGROUND_REMOVE);
        Player.setPropertyChangedListener(null);

        if (audioFocusHandler != null) {
            audioFocusHandler.release();
            audioFocusHandler = null;
        }

        if (mediaSession != null) {
            mediaSession.setActive(false);
            mediaSession.release();
        }
        mediaSession = null;

        clearCoverCache();

        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onSignalEmitted(String property, Boolean shouldUpdate)
    {
        if (Player.POSITION_PROPERTY.equals(property)) {
            if (mediaSession != null) {
                mediaSession.setPlaybackState(buildPlaybackState());
            }
            return;
        }

        Log.d(TAG, "onSignalEmitted: " + property + " - shouldUpdate:" + shouldUpdate);

        if (audioFocusHandler != null && Player.PLAYBACK_STATE_PROPERTY.equals(property)) {
            audioFocusHandler.onPlaybackStateChanged(Player.playbackState);
        }
        if (Boolean.TRUE.equals(shouldUpdate)) {
            updateMediaSession();
            refreshNotification();
        }
    }

    private void startForegroundWithNotification()
    {
        final int type = Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q ? ServiceInfo.FOREGROUND_SERVICE_TYPE_MEDIA_PLAYBACK : 0;
        ServiceCompat.startForeground(this, NOTIFICATION_ID, buildNotification(), type);
    }

    private void refreshNotification()
    {
        final NotificationManager notificationManager = getSystemService(NotificationManager.class);

        if (Player.playbackState == Player.PlaybackState.PlayingState) {
            pendingForegroundStart = false;
            mainHandler.removeCallbacks(stopIfIdle);
            startForegroundWithNotification();
        } else if (Player.playbackState == Player.PlaybackState.PausedState) {
            pendingForegroundStart = false;
            mainHandler.removeCallbacks(stopIfIdle);
            ServiceCompat.stopForeground(this, ServiceCompat.STOP_FOREGROUND_DETACH);
            if (notificationManager != null) {
                notificationManager.notify(NOTIFICATION_ID, buildNotification());
            }
        } else if (pendingForegroundStart) {
            pendingForegroundStart = false;
            startForegroundWithNotification();
        } else {
            ServiceCompat.stopForeground(this, ServiceCompat.STOP_FOREGROUND_REMOVE);
            if (notificationManager != null) {
                notificationManager.cancel(NOTIFICATION_ID);
            }
            clearCoverCache();
        }
    }

    private Bitmap coverBitmap()
    {
        final String url = Player.metadata.albumCover;
        if (url.isEmpty()) {
            return null;
        }

        if (cachedCover == null || !url.equals(cachedCoverUrl)) {
            cachedCoverUrl = url;
            cachedCover = CoverImageProvider.contentThumbnail(this, url, COVER_SIZE, COVER_SIZE);
            cachedCoverColorValid = false;
        }

        return cachedCover;
    }

    private int coverColor()
    {
        final Bitmap cover = coverBitmap();
        if (cover == null) {
            return 0;
        }

        if (!cachedCoverColorValid) {
            cachedCoverColor = Palette.from(cover).generate().getVibrantColor(0);
            cachedCoverColorValid = true;
        }

        return cachedCoverColor;
    }

    private void clearCoverCache()
    {
        cachedCoverUrl = null;
        cachedCover = null;
        cachedCoverColor = 0;
        cachedCoverColorValid = false;
    }

    private Notification buildNotification()
    {
        final boolean stopped = Player.playbackState == Player.PlaybackState.StoppedState;

        Intent notificationIntent = new Intent(this, ElisaActivity.class);
        PendingIntent pendingIntent = PendingIntentCompat.getActivity(this, 0, notificationIntent, 0, false);

        NotificationCompat.Builder notificationBuilder = new NotificationCompat.Builder(this, CHANNEL_ID)
                .setSmallIcon(R.mipmap.ic_notification_elisa)
                .setVisibility(NotificationCompat.VISIBILITY_PUBLIC)
                .setPriority(NotificationCompat.PRIORITY_DEFAULT)
                .setContentTitle(stopped || Player.metadata.title.isEmpty() ? "Elisa" : Player.metadata.title)
                .setContentText(stopped ? "" : Player.metadata.artist)
                .setContentIntent(pendingIntent)
                .setOngoing(Player.playbackState == Player.PlaybackState.PlayingState);

        if (stopped) {
            return notificationBuilder.build();
        }

        notificationBuilder
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

        final Bitmap cover = coverBitmap();
        if (cover != null) {
            notificationBuilder.setLargeIcon(cover);
            notificationBuilder.setColorized(true);
            notificationBuilder.setColor(coverColor());
        }

        MediaStyle style = new MediaStyle();
        style.setShowActionsInCompactView(0, 1, 2);
        if (mediaSession != null) {
            style.setMediaSession(mediaSession.getSessionToken());
        }
        notificationBuilder.setStyle(style);

        return notificationBuilder.build();
    }

    private MediaMetadataCompat buildMediaMetadata()
    {
        if (Player.playbackState == Player.PlaybackState.StoppedState) {
            return null;
        }

        return mediaMetadataBuilder
                .putBitmap(MediaMetadataCompat.METADATA_KEY_ART, coverBitmap())
                .putText(MediaMetadataCompat.METADATA_KEY_ALBUM, Player.metadata.albumName)
                .putText(MediaMetadataCompat.METADATA_KEY_ARTIST, Player.metadata.artist)
                .putLong(MediaMetadataCompat.METADATA_KEY_DURATION, Player.metadata.duration)
                .putText(MediaMetadataCompat.METADATA_KEY_TITLE, Player.metadata.title)
                .build();
    }

    private PlaybackStateCompat buildPlaybackState()
    {
        long actions = PlaybackStateCompat.ACTION_PLAY |
                       PlaybackStateCompat.ACTION_PLAY_PAUSE |
                       PlaybackStateCompat.ACTION_PAUSE |
                       PlaybackStateCompat.ACTION_SKIP_TO_PREVIOUS |
                       PlaybackStateCompat.ACTION_SKIP_TO_NEXT |
                       PlaybackStateCompat.ACTION_SEEK_TO;

        int state = PlaybackStateCompat.STATE_STOPPED;
        float speed = 0f;
        if (Player.playbackState == Player.PlaybackState.PlayingState) {
            state = PlaybackStateCompat.STATE_PLAYING;
            speed = 1f;
        } else if (Player.playbackState == Player.PlaybackState.PausedState) {
            state = PlaybackStateCompat.STATE_PAUSED;
            speed = 0f;
        }

        return playbackStateBuilder
                .setState(state, Player.position, speed)
                .setActions(actions)
                .build();
    }

    private void updateMediaSession()
    {
        if (mediaSession == null) {
            return;
        }
        PlaybackStateCompat state = buildPlaybackState();
        mediaSession.setMetadata(buildMediaMetadata());
        mediaSession.setActive(Player.playbackState != Player.PlaybackState.StoppedState);
        mediaSession.setPlaybackState(state);
    }

    public static void createNotificationChannel(Context context)
    {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(
                    CHANNEL_ID,
                    "Elisa Background Playback",
                    NotificationManager.IMPORTANCE_LOW
            );

            NotificationManager notificationManager = context.getSystemService(NotificationManager.class);
            if (notificationManager != null) {
                notificationManager.createNotificationChannel(channel);
            }
        }
    }
}
