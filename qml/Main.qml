import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

ApplicationWindow {
	id: root
	title: "Tenkan"
	visible: true
	width: 1280
	height: 720
	flags: Qt.FramelessWindowHint

	Material.theme: Material.Dark
	Material.primary: "#1a1a27"
	Material.background: "#1e1e2d"
	Material.accent: "#a15bfc"

	header: AppHeader {}

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

	PackageStatusDialog {
		id: packageStatusDialog
		modal: true
		focus: true
		width: 400
		x: Math.round((root.width - width) / 2)
		y: Math.round(root.height / 6)
	}

	ColumnLayout {
		anchors {
			fill: parent
		}

		PackageList {
			id: packageList
		}
	}
}
