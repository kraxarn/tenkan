#pragma once

#include <QString>

#include "data/packagetype.hpp"
#include "data/packagestatus.hpp"

using Package = struct package_t
{
	QString name;
	QVersionNumber version;
	PackageType type;
	QString assignedTeam;
	PackageStatus status;
	QDateTime lastChecked;
	QString source;
	QString comment;
	QString filePath;
	QString fileName;
	QString repositoryId;
};
