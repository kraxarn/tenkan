import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Dialog {
	title: packageName || ""
	standardButtons: Dialog.Ok

	property string packageName
	property string packageInfo

	Label {
		text: packageInfo || ""
	}
}
