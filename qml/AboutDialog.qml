import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Dialog {
	title: "About"
	standardButtons: Dialog.Ok

	Grid {
		columns: 2
		spacing: 8

		Label {
			text: AppName
			font.pointSize: 12
		}

		Label {
			text: AppVersion
			font.pointSize: 12
		}

		Label {
			text: "Built"
			font.pointSize: 12
		}

		Label {
			text: BuildDate
			font.pointSize: 12
		}

		Label {
			text: "Qt"
			font.pointSize: 12
		}

		Label {
			text: QtVersion
			font.pointSize: 12
		}
	}
}
