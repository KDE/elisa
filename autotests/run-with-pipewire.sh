#!/bin/bash
#
# SPDX-FileCopyrightText: 2026 Ian Monroe <imonroe@kde.org>
# SPDX-License-Identifier: LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
#
# Manage a private PipeWire + WirePlumber session for audio tests on headless
# machines that have PipeWire installed but no running daemon (e.g. the KDE CI
# linux-qt6 image).
#
# Modes:
#   run-with-pipewire.sh start          Start daemons, wait for readiness, exit
#                                       leaving them running (used as a CTest
#                                       FIXTURES_SETUP test).
#   run-with-pipewire.sh stop           Stop any daemons script started.
#   run-with-pipewire.sh exec <cmd...>  Start daemons, run <cmd>, then stop.

set -u

if [ -z "${XDG_RUNTIME_DIR:-}" ]; then
    export XDG_RUNTIME_DIR="$(mktemp -d -t elisa-pipewire-XXXXXX)"
fi

MODE="${1:-}"
case "$MODE" in
    start|stop|exec) ;;
    *)
        echo "Usage: $0 start|stop|exec <cmd...>" >&2
        exit 2
        ;;
esac
shift

stop_daemons() {
    pkill -x wireplumber 2>/dev/null || true
    pkill -x pipewire 2>/dev/null || true
}

start_daemons() {
    stop_daemons
    rm -f "$XDG_RUNTIME_DIR/pipewire-0" "$XDG_RUNTIME_DIR/pipewire-0.lock"
    rm -f "$XDG_RUNTIME_DIR/pipewire-0-manager" "$XDG_RUNTIME_DIR/pipewire-0-manager.lock"
    rm -rf "$XDG_RUNTIME_DIR/pulse"

    local pw_conf pw_log wp_log
    pw_conf="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")/pipewire-test-daemon.conf"
    pw_log="$XDG_RUNTIME_DIR/pipewire.log"
    wp_log="$XDG_RUNTIME_DIR/wireplumber.log"

    pipewire -c "$pw_conf" > "$pw_log" 2>&1 &
    local pw_pid=$!

    for ((i = 0; i < 100; ++i)); do
        [ -S "$XDG_RUNTIME_DIR/pipewire-0" ] && break
        if ! kill -0 "$pw_pid" 2>/dev/null; then
            echo "PipeWire exited unexpectedly during startup" >&2
            cat "$pw_log" >&2
            exit 1
        fi
        sleep 0.1
    done

    if [ ! -S "$XDG_RUNTIME_DIR/pipewire-0" ]; then
        echo "PipeWire socket did not appear within 10 seconds" >&2
        cat "$pw_log" >&2
        kill "$pw_pid" 2>/dev/null || true
        exit 1
    fi

    wireplumber > "$wp_log" 2>&1 &
    local wp_pid=$!

    for ((i = 0; i < 100; ++i)); do
        [ -S "$XDG_RUNTIME_DIR/pulse/native" ] && break
        if ! kill -0 "$wp_pid" 2>/dev/null; then
            echo "WirePlumber exited unexpectedly during startup" >&2
            cat "$wp_log" >&2
            kill "$pw_pid" 2>/dev/null || true
            exit 1
        fi
        sleep 0.1
    done

    if [ ! -S "$XDG_RUNTIME_DIR/pulse/native" ]; then
        echo "PipeWire PulseAudio socket did not appear within 10 seconds" >&2
        cat "$pw_log" >&2
        cat "$wp_log" >&2
        kill "$wp_pid" "$pw_pid" 2>/dev/null || true
        exit 1
    fi


    # just waiting for the socket to exist isn't enough; WirePlumber still
    # needs to register the null audio sink node so playback streams can be
    # created. Poll until the auto_null sink appears.
    for ((i = 0; i < 100; ++i)); do
        pw-cli ls 2>/dev/null | grep -q "node.name.*auto_null" && break
        sleep 0.1
    done

    if ! pw-cli ls 2>/dev/null | grep -q "node.name.*auto_null"; then
        echo "PipeWire null audio sink did not appear within 10 seconds" >&2
        cat "$pw_log" >&2
        cat "$wp_log" >&2
        kill "$wp_pid" "$pw_pid" 2>/dev/null || true
        exit 1
    fi

    echo "PipeWire + WirePlumber started (PIDs $pw_pid, $wp_pid)"
}

case "$MODE" in
    start)
        start_daemons
        ;;
    stop)
        stop_daemons
        ;;
    exec)
        start_daemons
        "$@"
        exit_code=$?
        stop_daemons
        exit $exit_code
        ;;
esac
