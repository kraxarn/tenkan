import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import PackageTableModel
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
	Material.primary: "#1e1e2d"
	Material.background: "#1a1a27"
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

		ListView {
			id: packageList

			Layout.fillWidth: true
			Layout.fillHeight: true

			Component.onCompleted: {
				packageModel.loadItems()
			}

			model: PackageTableModel {
				id: packageModel
			}

			delegate: ItemDelegate {
				width: parent ? parent.width : 0

				RowLayout {
					anchors.fill: parent
					spacing: 32

					Label {
						Layout.fillWidth: true
						Layout.leftMargin: 16
						text: model.packageName || "<empty>"
					}

					Label {
						text: model.packageVersion || "<empty>"
					}

					ToolButton {
						icon.source: model.packageSource ? `qrc:/res/icon/${model.packageSource}.svg` : undefined
					}

					Label {
						text: model.assignedTeam || "<empty>"
					}

					Label {
						text: model.status || "<empty>"
					}

					Label {
						text: model.lastChecked || "<empty>"
					}

					Label {
						Layout.rightMargin: 16
						text: model.actions || "<empty>"
					}
				}
			}
		}
	}
}
