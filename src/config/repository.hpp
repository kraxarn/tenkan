#pragma once

#include <QString>

using RepositoryConfig = struct repository_config_t
{
	QString id;
	QStringList files;
};
