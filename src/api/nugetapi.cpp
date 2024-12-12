#include "api/nugetapi.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

NuGetApi::NuGetApi(QObject *parent)
	: Api(parent)
{
}

auto NuGetApi::baseUrl() const -> QString
{
	return QStringLiteral("https://api.nuget.org");
}

auto NuGetApi::headers() const -> QHttpHeaders
{
	return {};
}

void NuGetApi::info(const QString &packageName, const std::function<void(NuGetPackageInfo)> &callback) const
{
	const auto url = QStringLiteral("/v3/registration5-semver1/%1/index.json")
		.arg(packageName.toLower());

	const auto request = prepareRequest(url);
	auto *reply = http()->get(request);

	await(reply, [callback](const QByteArray &response)
	{
		const auto json = QJsonDocument::fromJson(response);

		const auto item = json[QStringLiteral("items")].toArray().last().toObject();
		const auto commitTimeStamp = item[QStringLiteral("commitTimeStamp")].toString();

		QString title;
		QString licenseExpression;

		if (item.contains(QStringLiteral("items")))
		{
			const auto itemItem = item[QStringLiteral("items")].toArray().last();
			const auto catalogEntry = itemItem[QStringLiteral("catalogEntry")].toObject();

			title = catalogEntry[QStringLiteral("title")].toString();
			licenseExpression = catalogEntry[QStringLiteral("licenseExpression")].toString();
		}

		callback({
			.upper = item[QStringLiteral("upper")].toString(),
			.commitTimeStamp = QDateTime::fromString(commitTimeStamp, Qt::ISODate),
			.title = title,
			.licenseExpression = licenseExpression,
		});
	});
}
