import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import WindowManager
import Greeter

ToolBar {
	RowLayout {
		anchors.fill: parent

		ToolButton {
			icon.source: "qrc:/res/icon/window-close.svg"
			onClicked: window.close()
			visible: Settings.titlebarPlacement === "left"
		}

		ToolButton {
			icon.source: "qrc:/res/icon/window-minimize.svg"
			onClicked: window.minimize()
			visible: Settings.titlebarPlacement === "left"
		}

		ToolSeparator {
			visible: Settings.titlebarPlacement === "left"
		}

		AppLogo {}

		Label {
			text: greeter.message
			font.bold: true

			Greeter {
				id: greeter
			}
		}

		WindowManager {
			id: window
		}

		MouseArea {
			Layout.fillHeight: true
			Layout.fillWidth: true
			z: 1

			onPressed: window.startMove()
			onDoubleClicked: window.maximize()
		}

		ToolButton {
			icon.source: "qrc:/res/icon/filter-outline.svg"
		}

		ToolSeparator {
			visible: Settings.titlebarPlacement === "right"
		}

		ToolButton {
			icon.source: "qrc:/res/icon/window-minimize.svg"
			onClicked: window.minimize()
			visible: Settings.titlebarPlacement === "right"
		}

		ToolButton {
			icon.source: "qrc:/res/icon/window-close.svg"
			onClicked: window.close()
			visible: Settings.titlebarPlacement === "right"
		}
	}
}
