#pragma once

#include <QString>

#include "config/project.hpp"

using DevOpsConfig = struct devops_config_t
{
	QString pat;
	QString organization;
	QList<ProjectConfig> projects;
};
