#pragma once

#include <QString>

#include "config/repository.hpp"

using DevOpsConfig = struct devops_config_t
{
	QString pat;
	QString organization;
	QString project;
	QList<RepositoryConfig> repositories;
};
