/*
   SPDX-FileCopyrightText: 2024 (c) Jack Hill <jackhill3103@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "taskbarmanager.h"
#include "windowsLogging.h"

#include <QAbstractEventDispatcher>
#include <QAbstractNativeEventFilter>
#include <QQuickWindow>

#include <Shobjidl.h>
#include <WinDef.h>
#include <Windows.h>

constexpr int numOfButtons = 3;

#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
using namespace Qt::Literals::StringLiterals;

static HICON hiconFromTheme(const QString &iconName)
#else
static HICON hiconFromTheme(const QIcon::ThemeIcon iconName)
#endif
{
    const auto width = GetSystemMetrics(SM_CXSMICON);
    const auto height = GetSystemMetrics(SM_CYSMICON);
    return QIcon::fromTheme(iconName).pixmap(width, height).toImage().toHICON();
}

class TaskBarManagerPrivate
{
public:

    ITaskbarList3 *taskBar = nullptr;
    HWND hwnd = nullptr;

    /**
     * Whether the toolbar has been added to the taskbar
     */
    bool addedThumbButtons = false;

    QMediaPlayer::PlaybackState playbackState = QMediaPlayer::StoppedState;

    bool showProgress = false;
    qlonglong progressMaximum = 0;
    qlonglong progressValue = 0;

#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
    HICON skipBackwardIcon = hiconFromTheme(u"media-skip-backward"_s);
    HICON skipForwardIcon = hiconFromTheme(u"media-skip-forward"_s);
    HICON playIcon = hiconFromTheme(u"media-playback-start"_s);
    HICON pauseIcon = hiconFromTheme(u"media-playback-pause"_s);
#else
    HICON skipBackwardIcon = hiconFromTheme(QIcon::ThemeIcon::MediaSkipBackward);
    HICON skipForwardIcon = hiconFromTheme(QIcon::ThemeIcon::MediaSkipForward);
    HICON playIcon = hiconFromTheme(QIcon::ThemeIcon::MediaPlaybackStart);
    HICON pauseIcon = hiconFromTheme(QIcon::ThemeIcon::MediaPlaybackPause);
#endif

    /**
     * Contains the current state of each button
     */
    struct ButtonData {
        HICON icon = nullptr;
        bool enabled = false;
    };
    std::array<ButtonData, numOfButtons> buttonData = {{
        {skipBackwardIcon, false},
        {playIcon, false},
        {skipForwardIcon, false}
    }};

    /**
     * For convenience when reading code
     */
    enum ButtonID {
        SkipBackward = 0,
        TogglePlayback = 1,
        SkipForward = 2,
    };

    TaskBarManagerPrivate()
    {
        HRESULT result = CoCreateInstance(CLSID_TaskbarList,
                                          nullptr,
                                          CLSCTX_INPROC_SERVER,
                                          IID_PPV_ARGS(&taskBar));
        if (FAILED(result)) {
            taskBar = nullptr;
            qCWarning(orgKdeElisaWindows) << "Failed to create Windows taskbar instance";
            return;
        }
    }

    ~TaskBarManagerPrivate()
    {
        if (taskBar) {
            taskBar->Release();
        }
        DestroyIcon(skipForwardIcon);
        DestroyIcon(skipBackwardIcon);
        DestroyIcon(pauseIcon);
        DestroyIcon(playIcon);
    }

    /**
     * Update the native window handle for the taskbar instance
     */
    void setHWND(HWND newHwnd)
    {
        if (hwnd == newHwnd) {
            return;
        }

        hwnd = newHwnd;
        addedThumbButtons = false;
        if (hwnd) {
            updateProgressFlags();
            updateProgressValue();
            setupTaskBarManagerButtons();
        }
    }

    /**
     * Update the taskbar progress flags
     */
    void updateProgressFlags()
    {
        if (taskBar && hwnd) {
            taskBar->SetProgressState(hwnd, progressFlags());
        }
    }

    /**
     * Update the taskbar progress value, only if the taskbar is showing progress
     */
    void updateProgressValue()
    {
        if (taskBar && hwnd && showProgress && playbackState != QMediaPlayer::StoppedState) {
            taskBar->SetProgressValue(hwnd, progressValue, progressMaximum);
        }
    }

    /**
     * Update the state of a single toolbutton
     */
    void updateButton(const ButtonID buttonId)
    {
        if (!taskBar || !hwnd || !addedThumbButtons) {
            return;
        }
        THUMBBUTTON thumbButtons[1];
        initThumbButton(thumbButtons, buttonId);
        taskBar->ThumbBarUpdateButtons(hwnd, 1, thumbButtons);
    }

private:
    /**
     * Convert playbackState into windows taskbar flags
     */
    TBPFLAG progressFlags() const
    {
        if (!showProgress) {
            return TBPF_NOPROGRESS;
        }

        switch (playbackState) {
        case QMediaPlayer::StoppedState:
            return TBPF_NOPROGRESS;
        case QMediaPlayer::PlayingState:
            return TBPF_NORMAL;
        case QMediaPlayer::PausedState:
            return TBPF_PAUSED;
        default:
            return TBPF_NOPROGRESS;
        }
    }

    /**
     * Initialize a single toolbutton
     */
    void initThumbButton(THUMBBUTTON *button, const ButtonID id)
    {
        button->dwMask = THB_ICON | THB_FLAGS;
        button->iId = id;
        button->hIcon = buttonData.at(id).icon;
        button->dwFlags = buttonData.at(id).enabled ? THBF_ENABLED : THBF_DISABLED;
    }

    /**
     * Initialize the toolbar
     */
    void setupTaskBarManagerButtons()
    {
        if (!taskBar || !hwnd || addedThumbButtons) {
            return;
        }

        THUMBBUTTON thumbButtons[numOfButtons];
        THUMBBUTTON *currButton = thumbButtons;
        for (int i = 0; i < numOfButtons; ++i) {
            initThumbButton(currButton++, static_cast<ButtonID>(i));
        }

        // win32 api reference says we should pass &thumbButtons but this doesn't compile
        HRESULT result = taskBar->ThumbBarAddButtons(hwnd, numOfButtons, thumbButtons);
        if (FAILED(result)) {
            qCWarning(orgKdeElisaWindows) << "Failed to create Windows taskbar buttons";
        } else {
            addedThumbButtons = true;
        }
    }
};

TaskBarManager::TaskBarManager(QQuickItem *parent)
    : QQuickItem(parent)
    , d(std::make_unique<TaskBarManagerPrivate>())
{
    QAbstractEventDispatcher::instance()->installNativeEventFilter(this);

    connect(this, &QQuickItem::windowChanged, this, [this]() {
        d->setHWND(window() ? reinterpret_cast<HWND>(window()->winId()) : nullptr);
    });
}

TaskBarManager::~TaskBarManager() = default;

bool TaskBarManager::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *)
{
    if (eventType == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG *>(message);
        if (msg->hwnd != d->hwnd || msg->message != WM_COMMAND || HIWORD(msg->wParam) != THBN_CLICKED) {
            return false;
        }

        switch (LOWORD(msg->wParam)) {
        case TaskBarManagerPrivate::SkipBackward:
            Q_EMIT skipBackward();
            return true;
        case TaskBarManagerPrivate::TogglePlayback:
            Q_EMIT togglePlayback();
            return true;
        case TaskBarManagerPrivate::SkipForward:
            Q_EMIT skipForward();
            return true;
        }
    }
    return false;
}

QMediaPlayer::PlaybackState TaskBarManager::playbackState() const
{
    return d->playbackState;
}

bool TaskBarManager::showProgress() const
{
    return d->showProgress;
}

qulonglong TaskBarManager::progressMaximum() const
{
    return d->progressMaximum;
}

qulonglong TaskBarManager::progressValue() const
{
    return d->progressValue;
}

bool TaskBarManager::canSkipBackward() const
{
    return d->buttonData.at(TaskBarManagerPrivate::SkipBackward).enabled;
}

bool TaskBarManager::canSkipForward() const
{
    return d->buttonData.at(TaskBarManagerPrivate::SkipForward).enabled;
}

bool TaskBarManager::canTogglePlayback() const
{
    return d->buttonData.at(TaskBarManagerPrivate::TogglePlayback).enabled;
}

void TaskBarManager::setPlaybackState(const QMediaPlayer::PlaybackState newPlaybackState)
{
    if (d->playbackState == newPlaybackState) {
        return;
    }

    d->playbackState = newPlaybackState;
    Q_EMIT playbackStateChanged();

    d->buttonData.at(TaskBarManagerPrivate::TogglePlayback).icon =
        d->playbackState == QMediaPlayer::PlayingState ? d->pauseIcon : d->playIcon;
    d->updateButton(TaskBarManagerPrivate::TogglePlayback);
    d->updateProgressFlags();
}

void TaskBarManager::setShowProgress(const bool showProgress)
{
    if (d->showProgress == showProgress) {
        return;
    }

    d->showProgress = showProgress;
    Q_EMIT showProgressChanged();

    d->updateProgressFlags();
}

void TaskBarManager::setProgressMaximum(const qlonglong newMaximum)
{
    if (d->progressMaximum == newMaximum) {
        return;
    }

    d->progressMaximum = newMaximum;
    Q_EMIT progressMaximumChanged();

    if (d->progressValue > d->progressMaximum) {
        d->progressValue = d->progressMaximum;
        Q_EMIT progressValueChanged();
    }

    d->updateProgressValue();
}

void TaskBarManager::setProgressValue(const qlonglong newValue)
{
    if (d->progressValue == newValue) {
        return;
    }

    d->progressValue = newValue < d->progressMaximum ? newValue : d->progressMaximum;
    Q_EMIT progressValueChanged();

    d->updateProgressValue();
}

void TaskBarManager::setCanSkipBackward(const bool canSkip)
{
    if (d->buttonData[TaskBarManagerPrivate::SkipBackward].enabled == canSkip) {
        return;
    }

    d->buttonData[TaskBarManagerPrivate::SkipBackward].enabled = canSkip;
    Q_EMIT canSkipBackwardChanged();

    d->updateButton(TaskBarManagerPrivate::SkipBackward);
}

void TaskBarManager::setCanSkipForward(const bool canSkip)
{
    if (d->buttonData[TaskBarManagerPrivate::SkipForward].enabled == canSkip) {
        return;
    }

    d->buttonData[TaskBarManagerPrivate::SkipForward].enabled = canSkip;
    Q_EMIT canSkipForwardChanged();

    d->updateButton(TaskBarManagerPrivate::SkipForward);
}

void TaskBarManager::setCanTogglePlayback(const bool canToggle)
{
    if (d->buttonData[TaskBarManagerPrivate::TogglePlayback].enabled == canToggle) {
        return;
    }

    d->buttonData[TaskBarManagerPrivate::TogglePlayback].enabled = canToggle;
    Q_EMIT canTogglePlaybackChanged();

    d->updateButton(TaskBarManagerPrivate::TogglePlayback);
}

#include "moc_taskbarmanager.cpp"
