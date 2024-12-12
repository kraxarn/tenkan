#pragma once

#include <QString>

using NuGetPackageInfo = struct nuget_package_info_t
{
	QString id;
	QString version;
	QDateTime published;
	QString title;
	QString licenseExpression;
};
