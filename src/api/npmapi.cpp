#include "api/npmapi.hpp"

#include <QJsonDocument>
#include <QJsonObject>

NpmApi::NpmApi(QObject *parent)
	: Api(parent)
{
}

auto NpmApi::baseUrl() const -> QString
{
	return QStringLiteral("https://registry.npmjs.com");
}

auto NpmApi::headers() const -> QHttpHeaders
{
	return {};
}

void NpmApi::info(const QString &packageName, const std::function<void(NpmPackageInfo)> &callback) const
{
	const auto url = QStringLiteral("/%1").arg(packageName);
	const auto request = prepareRequest(url);
	auto *reply = http()->get(request);

	await(reply, [callback](const QByteArray &response)
	{
		const auto json = QJsonDocument::fromJson(response);

		const auto latest = json[QStringLiteral("dist-tags")][QStringLiteral("latest")].toString();
		const auto version = json[QStringLiteral("versions")][latest].toObject();

		const auto time = json[QStringLiteral("time")].toObject();
		const auto modified = time[QStringLiteral("modified")].toString();

		callback({
			.name = json[QStringLiteral("name")].toString(),
			.version = latest,
			.license = json[QStringLiteral("license")].toString(),
			.modified = QDateTime::fromString(modified, Qt::ISODate),
			.deprecated = version[QStringLiteral("deprecated")].toString(),
		});
	});
}
