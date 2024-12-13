#pragma once

#include <QString>

using CosmosDbConfig = struct cosmos_db_config_t
{
	QString key;
	QString name;
	QString database;
	QString container;
};
