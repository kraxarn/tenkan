import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

import Settings

Dialog {
	title: "Settings"
	standardButtons: Dialog.Ok

	Column {
		RowLayout {
			spacing: 12

			Label {
				Layout.alignment: Qt.AlignVCenter
				text: "Titlebar placement"
			}

			ComboBox {
				model: [
					"right",
					"left",
				]
				Component.onCompleted: currentIndex = indexOfValue(Settings.titlebarPlacement)
				onActivated: Settings.titlebarPlacement = currentValue
			}
		}
	}
}
