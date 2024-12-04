#include "api/aikidoapi.hpp"

#include <QJsonObject>
#include <QJsonArray>

#include "parser/csvparser.hpp"

AikidoApi::AikidoApi(const Config &config, QObject *parent)
	: Api(parent),
	config(config.aikido())
{
}

auto AikidoApi::baseUrl() const -> QString
{
	return QStringLiteral("https://app.aikido.dev/api");
}

auto AikidoApi::headers() const -> QHttpHeaders
{
	QHttpHeaders headers;

	if (token.isEmpty())
	{
		const auto basicToken = QStringLiteral("%1:%2")
			.arg(config.clientId, config.clientSecret)
			.toUtf8()
			.toBase64();

		headers.append("Authorization", QStringLiteral("Basic %1")
			.arg(QString::fromUtf8(basicToken)));
	}
	else
	{
		headers.append("Authorization", QStringLiteral("Bearer %1").arg(token));
	}


	headers.append("Content-Type", "application/json");
	return headers;
}

void AikidoApi::auth(const std::function<void(QString)> &callback)
{
	// Force fetching new token
	token = QString();

	const auto request = prepareRequest(QStringLiteral("/oauth/token"));

	QJsonObject body;
	body[QStringLiteral("grant_type")] = QStringLiteral("client_credentials");
	const auto data = QJsonDocument(body).toJson();

	auto *reply = http()->post(request, data);
	await(reply, [this, callback](const QByteArray &response)
	{
		const auto json = QJsonDocument::fromJson(response);
		const auto accessToken = json[QStringLiteral("access_token")].toString();

		token = accessToken;
		callback(accessToken);
	});
}

void AikidoApi::packages(qint32 codeRepoId, const std::function<void(QList<AikidoPackage>)> &callback)
{
	if (token.isEmpty())
	{
		auth([this, codeRepoId, callback]([[maybe_unused]] const QString &token)
		{
			packages(codeRepoId, callback);
		});
		return;
	}

	const auto url = QStringLiteral("/public/v1/repositories/code/%1/licenses/export")
		.arg(codeRepoId);

	const auto request = prepareRequest(url);
	auto *reply = http()->get(request);

	await(reply, [callback](const QByteArray &response)
	{
		const CsvParser parser(response);
		const auto rows = parser.rows();

		QList<AikidoPackage> packages;
		packages.reserve(rows.size());

		for (const auto &row: rows)
		{
			qsizetype i = 0;

			packages.append({
				.packageName = row.at(i++),
				.licenseType = row.at(i++),
				.language = row.at(i++),
				.risk = row.at(i++),
				.note = row.at(i++),
				.repo = row.at(i++),
				.container = row.at(i++),
				.location = row.at(i++),
				.orgName = row.at(i),
			});
		}

		callback(std::move(packages));
	});
}

void AikidoApi::repositories(const std::function<void(QList<AikidoCodeRepository>)> &callback)
{
	if (token.isEmpty())
	{
		auth([this, callback]([[maybe_unused]] const QString &token)
		{
			repositories(callback);
		});
		return;
	}

	const auto url = QStringLiteral("/public/v1/repositories/code");
	const auto request = prepareRequest(url);
	auto *reply = http()->get(request);

	await(reply, [callback](const QByteArray &response)
	{
		const auto json = QJsonDocument::fromJson(response);
		QList<AikidoCodeRepository> repositories;

		for (const auto &value: json.array())
		{
			repositories.append({
				.id = value[QStringLiteral("id")].toInt(),
				.name = value[QStringLiteral("name")].toString(),
				.externalRepoId = value[QStringLiteral("external_repo_id")].toString(),
				.provider = value[QStringLiteral("provider")].toString(),
				.active = value[QStringLiteral("active")].toBool(),
			});
		}

		callback(std::move(repositories));
	});
}
