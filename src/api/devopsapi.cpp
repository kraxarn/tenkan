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

auto DevOpsApi::repositoryId() const -> QString
{
	// TODO: Currently, we only support one repository
	return config.repositories.at(0).id;
}

auto DevOpsApi::projects() const -> const QStringList &
{
	// TODO: Currently, we only support one repository
	return config.repositories.at(0).files;
}

void DevOpsApi::getFileContent(const QString &path, const std::function<void(QByteArray)> &callback) const
{
	const auto url = QStringLiteral("/git/repositories/%1/items?path=%2")
		.arg(repositoryId(), path);

	const auto request = prepareRequest(url);
	auto *reply = http()->get(request);

	await(reply, callback);
}

void DevOpsApi::getPackageReferences(const std::function<void(QList<DotNet::PackageReference>)> &callback) const
{
	for (const auto &project: projects())
	{
		if (!project.endsWith(".csproj"))
		{
			continue;
		}

		getFileContent(project, [callback](const QByteArray &response)
		{
			CsProjParser parser(response);
			callback(parser.getPackageReferences());
		});
	}
}
