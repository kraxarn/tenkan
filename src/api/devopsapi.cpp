#include "api/devopsapi.hpp"

#include <QNetworkReply>

#include "parser/csprojparser.hpp"

DevOpsApi::DevOpsApi(const Config &config, QObject *parent)
	: Api(parent),
	config(config.devops())
{
}

auto DevOpsApi::baseUrl() const -> QString
{
	return QStringLiteral("https://dev.azure.com/%1/%2/_apis")
		.arg(config.organization, config.project);
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
	const auto url = QStringLiteral("/git/repositories/%1/items?path=%2")
		.arg(repositoryId, path);

	const auto request = prepareRequest(url);
	auto *reply = http()->get(request);

	await(reply, callback);
}

void DevOpsApi::getPackageReferences(const QString &repositoryId,
	const std::function<void(QList<DotNet::PackageReference>)> &callback) const
{
	for (const auto &repository: config.repositories)
	{
		if (repository.id != repositoryId)
		{
			continue;
		}

		for (const auto &path: repository.files)
		{
			if (!path.endsWith(".csproj"))
			{
				continue;
			}

			getFileContent(repository.id, path, [callback](const QByteArray &response)
			{
				CsProjParser parser(response);
				callback(parser.getPackageReferences());
			});
		}

		break;
	}
}

auto DevOpsApi::repositoryIds() const -> QStringList
{
	QStringList items;
	items.reserve(config.repositories.size());

	for (const auto &repository: config.repositories)
	{
		items.append(repository.id);
	}

	return items;
}


auto DevOpsApi::repositoryFileCount(const QString &suffix) const -> qsizetype
{
	qsizetype count = 0;

	for (const auto &repository: config.repositories)
	{
		for (const auto &path: repository.files)
		{
			if (path.endsWith(suffix))
			{
				count++;
			}
		}
	}

	return count;
}
