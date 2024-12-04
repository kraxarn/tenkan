#pragma once

#include <QString>

using AikidoCodeRepository = struct aikido_code_repository_t
{
	qint32 id;
	QString name;
	QString externalRepoId;
	QString provider;
	bool active;
};
