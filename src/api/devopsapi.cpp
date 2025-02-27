#include "api/devopsapi.hpp"

#include <QNetworkReply>
#include <QJsonArray>
#include <QDir>

#include "parser/csprojparser.hpp"
#include "parser/packagejsonparser.hpp"

DevOpsApi::DevOpsApi(const Config &config, QObject *parent)
	: Api(parent),
	config(config.devops())
{
}

auto DevOpsApi::baseUrl() const -> QString
{
	return QStringLiteral("https://dev.azure.com/%1")
		.arg(config.organization);
}

auto DevOpsApi::headers() const -> QHttpHeaders
{
	QHttpHeaders headers;
	headers.append("Authorization", QStringLiteral("Basic %1").arg(accessToken()));
	headers.append("Content-Type", "application/json");
	return headers;
}

auto DevOpsApi::accessToken() const -> QString
{
	return QString::fromUtf8(QStringLiteral(":%1")
		.arg(config.pat).toUtf8().toBase64());
}

void DevOpsApi::getFileContent(const QString &projectId, const QString &repositoryId,
	const QString &path, const std::function<void(QByteArray)> &callback) const
{
	const auto url = QStringLiteral("/%1/_apis/git/repositories/%2/items?path=%3")
		.arg(projectId, repositoryId, path);

	const auto request = prepareRequest(url);
	auto *reply = http()->get(request);

	await(reply, callback);
}

void DevOpsApi::getPackageReferences(const QString &projectId, const QString &repositoryId,
	const QString &path, const std::function<void(QList<DotNet::PackageReference>)> &callback) const
{
	getFileContent(projectId, repositoryId, path, [callback](const QByteArray &response)
	{
		CsProjParser parser(response);
		callback(parser.getPackageReferences());
	});
}

void DevOpsApi::packages(const QString &projectId, const QString &repositoryId,
	const QString &path, const std::function<void(QList<NodeJs::Package>)> &callback) const
{
	getFileContent(projectId, repositoryId, path,
		[this, projectId, repositoryId, path, callback](const QByteArray &response)
		{
			QString lockName;
			switch (PackageJsonParser::packageManager(response))
			{
				case NodeJsPackageManager::Npm:
					lockName = "package-lock.json";
					break;

				case NodeJsPackageManager::Yarn:
					lockName = "yarn.lock";
					break;

				case NodeJsPackageManager::Unknown:
					qFatal() << "Unknown package manager in package.json";
					break;
			}

			const auto lockPath = QFileInfo(path).dir().filePath(lockName);
			getFileContent(projectId, repositoryId, lockPath,
				[response, callback](const QByteArray &lockResponse)
				{
					const PackageJsonParser parser(response, lockResponse);
					callback(parser.packages());
				});
		});
}


void DevOpsApi::teams(const QString &projectId, const std::function<void(QList<Team>)> &callback) const
{
	const auto url = QStringLiteral("/_apis/projects/%1/teams?api-version=7.1")
		.arg(projectId);

	const auto request = prepareRequest(url);
	auto *reply = http()->get(request);

	await(reply, [callback](const QByteArray &response)
	{
		const auto json = QJsonDocument::fromJson(response);
		const auto count = json[QStringLiteral("count")].toInt();

		QList<Team> teams;
		teams.reserve(count);

		for (const auto &value : json[QStringLiteral("value")].toArray())
		{
			teams.append({
				.id = value[QStringLiteral("id")].toString(),
				.name = value[QStringLiteral("name")].toString(),
				.description = value[QStringLiteral("description")].toString(),
			});
		}

		callback(std::move(teams));
	});
}

void DevOpsApi::pullRequests(const QString &repositoryId, const QString &packageName,
	const std::function<void(QList<PullRequest>)> &callback) const
{
	const auto url = QStringLiteral(
		"/_apis/git/repositories/%1/pullrequests?searchCriteria.title=%2&searchCriteria.status=active&api-version=7.1"
	).arg(repositoryId, packageName);

	const auto request = prepareRequest(url);
	auto *reply = http()->get(request);

	await(reply, [callback](const QByteArray &response)
	{
		const auto json = QJsonDocument::fromJson(response);
		const auto count = json[QStringLiteral("count")].toInt();

		QList<PullRequest> pullRequests;
		pullRequests.reserve(count);

		for (const auto &value : json[QStringLiteral("value")].toArray())
		{
			const auto creationDate = value[QStringLiteral("creationDate")].toString();

			pullRequests.append({
				.pullRequestId = value[QStringLiteral("pullRequestId")].toInt(),
				.creationDate = QDateTime::fromString(creationDate, Qt::ISODate),
			});
		}

		callback(std::move(pullRequests));
	});
}

auto DevOpsApi::repositoryFileCount() const -> qsizetype
{
	qsizetype count = 0;

	for (const auto &project : config.projects)
	{
		for (const auto &repository : project.repositories)
		{
			count += repository.files.size();
		}
	}

	return count;
}

auto DevOpsApi::repositoryCount() const -> qsizetype
{
	qsizetype count = 0;

	for (const auto &project : config.projects)
	{
		count += project.repositories.size();
	}

	return count;
}

auto DevOpsApi::projectIds() const -> QStringList
{
	QStringList ids;
	ids.reserve(config.projects.size());

	for (const auto &project : config.projects)
	{
		ids.append(project.id);
	}

	return ids;
}

auto DevOpsApi::repositoryIds(const QString &projectId) const -> QStringList
{
	for (const auto &project : config.projects)
	{
		if (project.id != projectId)
		{
			continue;
		}

		QStringList ids;
		ids.reserve(project.repositories.size());

		for (const auto &repository : project.repositories)
		{
			ids.append(repository.id);
		}

		return ids;
	}

	return {};
}


auto DevOpsApi::repositoryFiles(const QString &repositoryId) const -> QStringList
{
	for (const auto &project : config.projects)
	{
		for (const auto &repository : project.repositories)
		{
			if (repository.id != repositoryId)
			{
				continue;
			}

			QStringList files;
			files.reserve(repository.files.size());

			for (const auto &path : repository.files)
			{
				files.append(path);
			}

			return files;
		}
	}


	return {};
}
