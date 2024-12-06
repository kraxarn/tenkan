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
				textRole: "text"
				valueRole: "value"
				model: [
					{ value: "right", text: "Right (default)" },
					{ value: "left", text: "Left" },
				]
				Component.onCompleted: currentIndex = indexOfValue(Settings.titlebarPlacement)
				onActivated: Settings.titlebarPlacement = currentValue
			}
		}
	}
}
