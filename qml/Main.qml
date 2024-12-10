import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import WindowManager
import Greeter

ApplicationWindow {
	id: root
	title: "Tenkan"
	visible: true
	width: 1280
	height: 720
	flags: Qt.FramelessWindowHint

	Material.theme: Material.Dark
	Material.primary: "#1a1a27"
	Material.background: "#1e1e2d"
	Material.accent: "#a15bfc"

	header: ToolBar {
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

	AboutDialog {
		id: aboutDialog
		modal: true
		focus: true
		x: Math.round((root.width - width) / 2)
		y: Math.round(root.height / 6)
	}

	SettingsDialog {
		id: settingsDialog
		modal: true
		focus: true
		x: Math.round((root.width - width) / 2)
		y: Math.round(root.height / 6)
	}

	ColumnLayout {
		anchors {
			fill: parent
		}

		PackageList {
			id: packageList
		}
	}
}
