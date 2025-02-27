#pragma once

#include "config.hpp"
#include "data/dotnet/packagereference.hpp"
#include "data/nodejs/package.hpp"
#include "api/api.hpp"
#include "data/team.hpp"
#include "data/pullrequest.hpp"

class DevOpsApi : public Api
{
	Q_OBJECT

public:
	DevOpsApi(const Config &config, QObject *parent);

	void getPackageReferences(const QString &projectId, const QString &repositoryId, const QString &path,
		const std::function<void(QList<DotNet::PackageReference>)> &callback) const;

	void packages(const QString &projectId, const QString &repositoryId, const QString &path,
		const std::function<void(QList<NodeJs::Package>)> &callback) const;

	[[nodiscard]]
	auto repositoryFileCount() const -> qsizetype;

	[[nodiscard]]
	auto repositoryCount() const -> qsizetype;

	[[nodiscard]]
	auto projectIds() const -> QStringList;

	[[nodiscard]]
	auto repositoryIds(const QString &projectId) const -> QStringList;

	[[nodiscard]]
	auto repositoryFiles(const QString &repositoryId) const -> QStringList;

	void teams(const QString &projectId, const std::function<void(QList<Team>)> &callback) const;

	void pullRequests(const QString &repositoryId, const QString &packageName,
		const std::function<void(QList<PullRequest>)> &callback) const;

protected:
	[[nodiscard]]
	auto baseUrl() const -> QString override;

	[[nodiscard]]
	auto headers() const -> QHttpHeaders override;

private:
	const DevOpsConfig config;

	void getFileContent(const QString &projectId, const QString &repositoryId,
		const QString &path, const std::function<void(QByteArray)> &callback) const;

	auto accessToken() const -> QString;
};
