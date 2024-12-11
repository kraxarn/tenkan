#pragma once

#include <QString>
#include <QDateTime>

using NpmPackageInfo = struct npm_package_info_t
{
	QString name;
	QString version;
	QString license;
	QDateTime modified;
	QString deprecated;
};
