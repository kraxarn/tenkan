import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

ToolButton {
	icon.source: "qrc:/res/icon/package-variant.svg"
	onClicked: appMenu.open()

	Menu {
		id: appMenu

		MenuItem {
			text: "Settings..."
			onClicked: settingsDialog.open()
		}

		MenuItem {
			text: "About..."
			onClicked: aboutDialog.open()
		}

		MenuSeparator {}

		MenuItem {
			text: "Quit"
		}
	}
}
