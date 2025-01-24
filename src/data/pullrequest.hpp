#pragma once

#include <QString>
#include <QDateTime>

using PullRequest = struct pull_request_t
{
	int pullRequestId;
	QDateTime creationDate;
};
