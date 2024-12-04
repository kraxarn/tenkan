#include "devops.hpp"

#include <QNetworkReply>

#include "parser/csprojparser.hpp"

DevOps::DevOps(const Config &config, QObject *parent)
	: QObject(parent),
	config(config.devops()),
	manager(new QNetworkAccessManager(this))
{
}

auto DevOps::baseUrl() const -> QString
{
	return QStringLiteral("https://dev.azure.com/%1/%2/_apis")
		.arg(config.organization, config.project);
}

auto DevOps::accessToken() const -> QString
{
	return QString::fromUtf8(QStringLiteral(":%1")
		.arg(config.pat).toUtf8().toBase64());
}

auto DevOps::repositoryId() const -> QString
{
	// TODO: Currently, we only support one repository
	return config.repositories.at(0).id;
}

auto DevOps::projects() const -> const QStringList &
{
	// TODO: Currently, we only support one repository
	return config.repositories.at(0).files;
}

auto DevOps::prepareRequest(const QString &url) const -> QNetworkRequest
{
	QNetworkRequest request;
	request.setUrl(baseUrl() + url);

	QHttpHeaders headers;
	headers.append("Authorization", QStringLiteral("Basic %1").arg(accessToken()));
	headers.append("Content-Type", "application/json");
	request.setHeaders(headers);

	return request;
}

void DevOps::await(QNetworkReply *reply, const std::function<void(QByteArray)> &callback) const
{
	connect(reply, &QNetworkReply::finished, this, [reply, callback]
	{
		if (reply->error() != QNetworkReply::NoError)
		{
			qWarning() << "Error:" << reply->errorString();
			return;
		}

		callback(reply->readAll());
		reply->deleteLater();
	});
}

void DevOps::getFileContent(const QString &path, const std::function<void(QByteArray)> &callback) const
{
	const auto url = QStringLiteral("/git/repositories/%1/items?path=%2")
		.arg(repositoryId(), path);

	const auto request = prepareRequest(url);
	auto *reply = manager->get(request);

	await(reply, callback);
}

void DevOps::getPackageReferences(const std::function<void(QList<DotNet::PackageReference>)> &callback) const
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
