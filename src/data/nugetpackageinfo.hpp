#pragma once

#include <QString>
#include <QDateTime>

using NuGetPackageInfo = struct nuget_package_info_t
{
	QString upper;
	QDateTime commitTimeStamp;
	QString title;
	QString licenseExpression;
};
