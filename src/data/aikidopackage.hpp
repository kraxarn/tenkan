#pragma once

#include <QString>

using AikidoPackage = struct aikido_package_t
{
	QString packageName;
	QString licenseType;
	QString language;
	QString risk;
	QString note;
	QString repo;
	QString container;
	QString location;
	QString orgName;
};
