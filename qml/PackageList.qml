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
		id: row
		width: parent ? Math.min(1280, parent.width) : 0

		anchors {
			horizontalCenter: parent ? parent.horizontalCenter : undefined
		}

		Component.onCompleted: {
			packageModel.updateStatus(model.packageName)
		}

		required property var model

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
				onClicked: Qt.openUrlExternally(model.packageUrl)
			}

			ToolButton {
				text: model.assignedTeam || "<empty>"
				onClicked: teamMenu.open()

				Menu {
					id: teamMenu

					Repeater {
						model: packageModel.teams

						MenuItem {
							text: modelData.name
							onTriggered: packageModel.assignTeam(row.model.packageName, modelData.id)
						}
					}
				}
			}

			ToolButton {
				icon.source: `qrc:/res/icon/${packageModel.getStatusIcon(model.status)}.svg`
				enabled: model.status > 1
				onClicked: {
					packageStatusDialog.packageName = model.packageName
					packageStatusDialog.packageInfo = packageModel.getPackageStatusInfo(model.packageName, model.status)
					packageStatusDialog.open()
				}

				ToolTip.visible: down
				ToolTip.text: packageModel.getStatusText(model.status)
			}

			Label {
				text: model.lastChecked || "<empty>"
			}

			ToolButton {
				Layout.rightMargin: 16
				icon.source: "qrc:/res/icon/dots-horizontal.svg"
				onClicked: actionsMenu.open()

				Menu {
					id: actionsMenu
					contentWidth: 350

					Repeater {
						model: row.model.filePaths

						MenuItem {
							text: `Open ${modelData.name}`
							icon.source: "qrc:/res/icon/file-code.svg"

							ToolTip.visible: down
							ToolTip.text: modelData.path
						}
					}
				}
			}
		}
	}
}
