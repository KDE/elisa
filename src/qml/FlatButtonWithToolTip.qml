/*
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>
   SPDX-FileCopyrightText: 2023 (c) ivan tkachenko <me@ratijas.tk>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Templates 2.15 as T
import org.kde.kirigami 2.15 as Kirigami
import org.kde.elisa 1.0

ToolButton {
    id: control

    enum MenuPolicy {
        /**
         * Menu opens on click, press or Enter/Return key press.
         */
        OnClickOrPressOrEnter = 1,
        /**
         * Menu opens on hold press only. Click does something different.
         */
        OnPressAndHold = 2
    }

    property /*MenuPolicy*/int menuPolicy: FlatButtonWithToolTip.MenuPolicy.OnPressAndHold

    property T.Popup menu

    // This is a guard against double-triggering when press is followed by
    // click. Click may also happen for different reasons, such as Enter key
    // press, so it is not always preceded by a press.
    property bool __suppressClick: false

    function __toggleMenu() {
        if (menu === null) {
            return;
        }
        if (menu.visible) {
            // Normally, this code path should be invoked, as a modal popup
            // would take focus away from this button and close itself on
            // click outside.
            if (menu instanceof T.Menu) {
                menu.dismiss();
            } else {
                menu.close();
            }
        } else {
            // Without modal, clicking on the button will not close the menu.
            menu.modal = true;
            menu.dim = false;

            if (menu instanceof T.Menu) {
                menu.popup(this);
            } else {
                menu.open();
            }

            // Manual popup positioning around this button. This is something
            // we should really be getting from QQC2 for free though.
            menu.x = Qt.binding(() => {
                const topLeft = mapToGlobal(0, 0);
                const bottomRight = mapToGlobal(width, height);

                if (!mirrored && bottomRight.x + menu.width > Overlay.overlay.width && menu.width < topLeft.x) {
                    return width - menu.width;
                } else {
                    return 0;
                }
            });
            menu.y = Qt.binding(() => {
                const topLeft = mapToGlobal(0, 0);
                const bottomRight = mapToGlobal(width, height);

                if (bottomRight.y + menu.height > Overlay.overlay.height && menu.height < topLeft.y) {
                    return -menu.height;
                } else {
                    return height;
                }
            });
        }
    }

    function __toggleMenuForPolicy(policy, suppressClick) {
        if (menu !== null && menuPolicy === policy) {
            if (!__suppressClick || menuPolicy === FlatButtonWithToolTip.MenuPolicy.OnPressAndHold) {
                __toggleMenu();
            }
            __suppressClick = suppressClick
        }
    }

    display: T.AbstractButton.IconOnly

    down: pressed || (menu !== null && menu.visible)

    ToolTip.visible: hovered
                     && text.length > 0
                     && display === T.AbstractButton.IconOnly
                     && (menu === null || !menu.visible)
    ToolTip.delay: Kirigami.Units.toolTipDelay
    ToolTip.text: text

    Accessible.role: (menu !== null && menuPolicy === FlatButtonWithToolTip.MenuPolicy.OnClickOrPressOrEnter)
        ? Accessible.ButtonMenu : Accessible.Button

    Accessible.onPressAction: clicked()

    Keys.onReturnPressed: action ? action.trigger() : clicked()
    Keys.onEnterPressed: action ? action.trigger() : clicked()

    onPressed: __toggleMenuForPolicy(FlatButtonWithToolTip.MenuPolicy.OnClickOrPressOrEnter, true)
    onClicked: __toggleMenuForPolicy(FlatButtonWithToolTip.MenuPolicy.OnClickOrPressOrEnter, false)
    onPressAndHold: __toggleMenuForPolicy(FlatButtonWithToolTip.MenuPolicy.OnPressAndHold, false)
    onReleased: control.__suppressClick = false;

    Connections {
        target: control.menu

        function onOpened() {
            // Press event is not always followed by click
            control.__suppressClick = false;
        }

        function onClosed() {
            control.__suppressClick = false;
            // break bindings, so they don't update while menu is invisible
            control.menu.x = control.menu.x;
            control.menu.y = control.menu.y;
        }
    }
}
