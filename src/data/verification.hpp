#pragma once

#include <QString>
#include <QDateTime>

using Verification = struct verification_t
{
	QString packageName;
	QString teamId;
	QDateTime timestamp;
};
