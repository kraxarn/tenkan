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

void DevOpsApi::getFileContent(const QString &repositoryId, const QString &path,
	const std::function<void(QByteArray)> &callback) const
{
	const auto url = QStringLiteral("/%1/_apis/git/repositories/%2/items?path=%3")
		.arg(config.project, repositoryId, path);

	const auto request = prepareRequest(url);
	auto *reply = http()->get(request);

	await(reply, callback);
}

void DevOpsApi::getPackageReferences(const QString &repositoryId, const QString &path,
	const std::function<void(QList<DotNet::PackageReference>)> &callback) const
{
	getFileContent(repositoryId, path, [callback](const QByteArray &response)
	{
		CsProjParser parser(response);
		callback(parser.getPackageReferences());
	});
}

void DevOpsApi::packages(const QString &repositoryId, const QString &path,
	const std::function<void(QList<NodeJs::Package>)> &callback) const
{
	getFileContent(repositoryId, path, [this, repositoryId, path, callback](const QByteArray &response)
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
		getFileContent(repositoryId, lockPath, [response, callback](const QByteArray &lockResponse)
		{
			const PackageJsonParser parser(response, lockResponse);
			callback(parser.packages());
		});
	});
}


void DevOpsApi::teams(const std::function<void(QList<Team>)> &callback) const
{
	const auto url = QStringLiteral("/_apis/projects/%1/teams?api-version=7.1")
		.arg(config.project);

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

auto DevOpsApi::repositoryFileCount() const -> qsizetype
{
	qsizetype count = 0;

	for (const auto &repository : config.repositories)
	{
		count += repository.files.size();
	}

	return count;
}

auto DevOpsApi::repositoryIds() const -> QStringList
{
	QStringList ids;
	ids.reserve(config.repositories.size());

	for (const auto &repository : config.repositories)
	{
		ids.append(repository.id);
	}

	return ids;
}


auto DevOpsApi::repositoryFiles(const QString &repositoryId) const -> QStringList
{
	for (const auto &repository : config.repositories)
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

	return {};
}
