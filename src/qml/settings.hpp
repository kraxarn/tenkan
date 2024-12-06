#pragma once

#include <qqmlintegration.h>
#include <QSettings>

class Settings : public QObject
{
	Q_OBJECT
	QML_SINGLETON
	QML_ELEMENT

	Q_PROPERTY(QString titlebarPlacement
		READ getTitlebarPlacement
		WRITE setTitlebarPlacement
		NOTIFY titlebarPlacementChanged
	)

public:
	explicit Settings(QObject *parent = nullptr);

	[[nodiscard]]
	auto getTitlebarPlacement() const -> QString;

	void setTitlebarPlacement(const QString &value);

signals:
	void titlebarPlacementChanged();

private:
	QSettings settings;
};
