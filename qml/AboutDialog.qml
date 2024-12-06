import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Dialog {
	title: "About"
	standardButtons: Dialog.Ok

	Column {
		Label {
			text: `${AppName} ${AppVersion}`
			font.pointSize: 12
		}

		Label {
			text: BuildDate
			font.pointSize: 12
		}

		Label {
			text: `Qt ${QtVersion}`
			font.pointSize: 12
		}
	}
}
