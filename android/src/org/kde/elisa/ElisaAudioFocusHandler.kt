/*
 * SPDX-FileCopyrightText: 2026 Ian Monroe <imonroe@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

package org.kde.elisa

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.media.AudioAttributes
import android.media.AudioFocusRequest
import android.media.AudioManager
import android.os.Handler
import android.os.Looper
import android.util.Log
import androidx.core.content.ContextCompat

class ElisaAudioFocusHandler(private val context: Context) {

    private val audioManager = context.getSystemService(AudioManager::class.java)

    private var pausedByFocusLoss = false
    private var playbackDelayed = false
    private var noisyReceiverRegistered = false

    private val isPlaying
        get() = Player.playbackState == Player.PlaybackState.PlayingState

    private val focusListener = AudioManager.OnAudioFocusChangeListener { change ->
        Log.d(TAG, "onAudioFocusChange: $change")
        when (change) {
            AudioManager.AUDIOFOCUS_LOSS -> {
                pausedByFocusLoss = false
                playbackDelayed = false
                if (isPlaying) {
                    Player.pause()
                }
            }
            AudioManager.AUDIOFOCUS_LOSS_TRANSIENT -> {
                if (isPlaying) {
                    pausedByFocusLoss = true
                    Player.pause()
                }
            }
            AudioManager.AUDIOFOCUS_GAIN -> {
                if (playbackDelayed) {
                    playbackDelayed = false
                    Player.play()
                } else if (pausedByFocusLoss) {
                    pausedByFocusLoss = false
                    Player.play()
                }
            }
        }
    }

    private val noisyReceiver =
        object : BroadcastReceiver() {
            override fun onReceive(receiverContext: Context, intent: Intent) {
                if (isPlaying) {
                    Log.d(TAG, "Audio becoming noisy; pausing playback")
                    Player.pause()
                }
            }
        }

    private val focusRequest: AudioFocusRequest =
        AudioFocusRequest.Builder(AudioManager.AUDIOFOCUS_GAIN)
            .setAudioAttributes(
                AudioAttributes.Builder()
                    .setUsage(AudioAttributes.USAGE_MEDIA)
                    .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                    .build(),
            )
            .setAcceptsDelayedFocusGain(true)
            .setOnAudioFocusChangeListener(focusListener, Handler(Looper.getMainLooper()))
            .build()

    fun onPlaybackStateChanged(state: Player.PlaybackState) {
        when (state) {
            Player.PlaybackState.PlayingState -> {
                if (!requestFocus()) {
                    Log.d(TAG, "Audio focus not granted; pausing playback")
                    Player.pause()
                }
                registerNoisyReceiver()
            }
            Player.PlaybackState.PausedState -> unregisterNoisyReceiver()
            Player.PlaybackState.StoppedState -> {
                abandonFocus()
                unregisterNoisyReceiver()
            }
        }
    }

    fun release() {
        abandonFocus()
        unregisterNoisyReceiver()
    }

    fun requestFocus(): Boolean {
        playbackDelayed = false
        return when (audioManager?.requestAudioFocus(focusRequest)) {
            AudioManager.AUDIOFOCUS_REQUEST_GRANTED -> true
            AudioManager.AUDIOFOCUS_REQUEST_DELAYED -> {
                playbackDelayed = true
                false
            }
            else -> {
                Log.w(TAG, "Audio focus request rejected")
                false
            }
        }
    }

    private fun abandonFocus() {
        pausedByFocusLoss = false
        playbackDelayed = false
        audioManager?.abandonAudioFocusRequest(focusRequest)
    }

    private fun registerNoisyReceiver() {
        if (!noisyReceiverRegistered) {
            ContextCompat.registerReceiver(
                context,
                noisyReceiver,
                IntentFilter(AudioManager.ACTION_AUDIO_BECOMING_NOISY),
                ContextCompat.RECEIVER_NOT_EXPORTED,
            )
            noisyReceiverRegistered = true
        }
    }

    private fun unregisterNoisyReceiver() {
        if (noisyReceiverRegistered) {
            runCatching { context.unregisterReceiver(noisyReceiver) }
                .onFailure { Log.w(TAG, "Cannot unregister the noisy audio receiver", it) }
            noisyReceiverRegistered = false
        }
    }

    companion object {
        private const val TAG = "ElisaAudioFocusHandler"
    }
}
