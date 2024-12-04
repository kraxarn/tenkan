import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import PackageTableModel
import WindowManager

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

	// menuBar: MenuBar {
	// 	Menu {
	// 		title: qsTr("&File")
	// 		Action { text: qsTr("&New...") }
	// 		Action { text: qsTr("&Open...") }
	// 		Action { text: qsTr("&Save") }
	// 		Action { text: qsTr("Save &As...") }
	// 		MenuSeparator { }
	// 		Action { text: qsTr("&Quit") }
	// 	}
	// 	Menu {
	// 		title: qsTr("&Edit")
	// 		Action { text: qsTr("Cu&t") }
	// 		Action { text: qsTr("&Copy") }
	// 		Action { text: qsTr("&Paste") }
	// 	}
	// 	Menu {
	// 		title: qsTr("&Help")
	// 		Action { text: qsTr("&About") }
	// 	}
	// }

	header: ToolBar {
		RowLayout {
			anchors.fill: parent

			ToolButton {
				icon.source: "qrc:/res/icon/package-variant.svg"
			}

			Label {
				text: "Welcome!"
				font.bold: true
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

			// Label {
			// 	text: "Title"
			// 	elide: Label.ElideRight
			// 	horizontalAlignment: Qt.AlignHCenter
			// 	verticalAlignment: Qt.AlignVCenter
			// }

			ToolButton {
				icon.source: "qrc:/res/icon/filter-outline.svg"
			}

			ToolSeparator {}

			ToolButton {
				icon.source: "qrc:/res/icon/window-minimize.svg"
				onClicked: window.minimize()
			}

			ToolButton {
				icon.source: "qrc:/res/icon/window-close.svg"
				onClicked: window.close()
			}
		}
	}

	// footer: Label {
	// 	text: "Loading packages..."
	// }

	// footer: TabBar {
	// 	id: bar
	// 	width: parent.width
	// 	TabButton {
	// 		text: qsTr("Home")
	// 	}
	// 	TabButton {
	// 		text: qsTr("Discover")
	// 	}
	// 	TabButton {
	// 		text: qsTr("Activity")
	// 	}
	// }

	// StackView {
	// 	anchors.fill: parent
	// }

	ColumnLayout {
		anchors {
			fill: parent
			// margins: 16
		}

		// Pane {
		// 	Layout.fillWidth: true
		//
		// 	background: Rectangle {
		// 		color: Material.primary
		// 	}
		// }

		// HorizontalHeaderView {
		// 	syncView: packageTable
		// 	z: 1
		// }

		ListView {
			id: packageList

			Layout.fillWidth: true
			Layout.fillHeight: true

			// property int foo

			Component.onCompleted: {
				packageModel.loadItems()
			}

			model: PackageTableModel {
				id: packageModel
			}

			// onWidthChanged: {
			// 	packageTable.foo = width
			// }

			// delegate: ItemDelegate {
			// 	implicitWidth: packageTable.width / 7
			// 	Label {
			// 		text: model.display
			// 	}
			// }

			// delegate: Pane {
			// 	// implicitWidth: packageTable.foo / 7
			//
			// 	Label {
			// 		text: display
			// 	}
			// }

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
