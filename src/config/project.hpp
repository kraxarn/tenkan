#pragma once

#include <QString>

#include "config/repository.hpp"

using ProjectConfig = struct project_config_t
{
	QString id;
	QList<RepositoryConfig> repositories;
};
