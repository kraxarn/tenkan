import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import PackageTableModel

ListView {
	Layout.fillWidth: true
	Layout.fillHeight: true

	ScrollBar.vertical: ScrollBar {}

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

			ToolButton {
				text: model.assignedTeam || "<empty>"
				onClicked: teamMenu.open()

				Menu {
					id: teamMenu

					MenuItem {
						text: "Nothing here"
						enabled: false
					}
				}
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
