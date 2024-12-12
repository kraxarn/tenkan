#pragma once

#include <QString>

using PackageInfo = struct package_info_t
{
	QString name;
	QVersionNumber version;
	QString license;
	QDateTime updated;
	bool deprecated;
};
