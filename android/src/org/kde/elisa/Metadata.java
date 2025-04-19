// SPDX-FileCopyrightText: 2025 (c) Pedro Nishiyama <nishiyama.v3@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

package org.kde.elisa;

public class Metadata
{
    final public String title;
    final public String artist;
    final public String albumName;
    final public String albumCover;
    final public long duration;

    public Metadata(String title, String artist, String albumName, String albumCover, long duration) {
        this.title = title;
        this.artist = artist;
        this.albumName = albumName;
        this.albumCover = albumCover;
        this.duration = duration;
    }

    public Metadata() {
        this.title = "";
        this.artist = "";
        this.albumName = "";
        this.albumCover = "";
        this.duration = 0;
    }
}