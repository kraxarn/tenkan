#include "qml/settings.hpp"

Settings::Settings(QObject *parent)
	: settings(parent)
{
}

auto Settings::getTitlebarPlacement() const -> QString
{
	return settings.value("titlebarPlacement", "right").toString();
}

void Settings::setTitlebarPlacement(const QString &value)
{
	settings.setValue("titlebarPlacement", value);
	emit titlebarPlacementChanged();
}
