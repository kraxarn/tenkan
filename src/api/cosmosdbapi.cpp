#include "api/cosmosdbapi.hpp"

#include <QJsonArray>
#include <QMessageAuthenticationCode>
#include <QJsonObject>

CosmosDbApi::CosmosDbApi(const Config &config, QObject *parent)
	: Api(parent),
	config(config.cosmosDb())
{
}

auto CosmosDbApi::baseUrl() const -> QString
{
	return QStringLiteral("https://%1.documents.azure.com")
		.arg(config.name);
}

auto CosmosDbApi::headers() const -> QHttpHeaders
{
	QHttpHeaders headers = requestHeaders;
	headers.append("Accept", "application/json");
	headers.append("x-ms-version", "2018-12-31");
	return headers;
}

void CosmosDbApi::databases()
{
	const auto date = dateStr();
	const auto auth = createAuthSignature(QStringLiteral("GET"), QStringLiteral("dbs"), {}, date);
	requestHeaders.append("x-ms-date", date);
	requestHeaders.append("authorization", auth);

	const auto request = prepareRequest(QStringLiteral("/dbs"));
	auto *reply = http()->get(request);

	await(reply, [](const QByteArray &response)
	{
		qInfo() << QString::fromUtf8(response);
	});
}

void CosmosDbApi::createDocument(const QString &id, const QString &value, const QDateTime &timestamp)
{
	const auto resourceLink = QStringLiteral("dbs/%1/colls/%2")
		.arg(config.database, config.container);

	const auto date = dateStr();

	const auto auth = createAuthSignature(QStringLiteral("POST"), QStringLiteral("docs"), resourceLink, date);
	requestHeaders.append("x-ms-date", date);
	requestHeaders.append("authorization", auth);
	requestHeaders.append("x-ms-documentdb-is-upsert", "True");
	requestHeaders.append("x-ms-documentdb-partitionkey", QStringLiteral("[\"%1\"]").arg(id));

	QJsonObject json;
	json[QStringLiteral("id")] = id;
	json[QStringLiteral("value")] = value;
	json[QStringLiteral("timestamp")] = timestamp.toString(Qt::ISODate);
	const auto body = QJsonDocument(json).toJson(QJsonDocument::Compact);

	const auto request = prepareRequest(QStringLiteral("/%1/docs").arg(resourceLink));
	auto *reply = http()->post(request, body);

	await(reply, []([[maybe_unused]] const QByteArray &response)
	{
	});
}

void CosmosDbApi::queryDocuments(const std::function<void(QList<QJsonObject>)> &callback)
{
	const auto resourceLink = QStringLiteral("dbs/%1/colls/%2")
		.arg(config.database, config.container);

	const auto date = dateStr();

	const auto auth = createAuthSignature(QStringLiteral("POST"), QStringLiteral("docs"), resourceLink, date);
	requestHeaders.append("x-ms-date", date);
	requestHeaders.append("authorization", auth);
	requestHeaders.append("x-ms-documentdb-isquery", "True");
	requestHeaders.append("x-ms-documentdb-partitionkey", QStringLiteral("[\"\"]"));

	const auto body = QStringLiteral("select * from c").toUtf8();

	const auto request = prepareRequest(QStringLiteral("/%1/docs").arg(resourceLink));
	auto *reply = http()->post(request, body);

	await(reply, [callback](const QByteArray &response)
	{
		const auto json = QJsonDocument::fromJson(response);
		const auto documents = json[QStringLiteral("Documents")].toArray();

		QList<QJsonObject> items;
		items.reserve(documents.size());

		for (const auto &document : documents)
		{
			items.append(document.toObject());
		}

		callback(items);
	});
}


auto CosmosDbApi::dateStr() -> QString
{
	return QDateTime::currentDateTimeUtc().toString(QStringLiteral("ddd, dd MMM yyyy hh:mm:ss 'GMT'"));
}

auto CosmosDbApi::createAuthSignature(const QString &method, const QString &resourceType,
	const QString &resourceLink, const QString &date) const -> QString
{
	const auto key = QByteArray::fromBase64(config.key.toUtf8());

	const auto payload = QStringLiteral("%1\n%2\n%3\n%4\n\n")
		.arg(method.toLower(), resourceType.toLower(), resourceLink, date.toLower());

	QMessageAuthenticationCode code(QCryptographicHash::Sha256, key);
	code.addData(payload.toUtf8());
	const auto payloadHash = code.result();

	const auto signature = QString::fromUtf8(payloadHash.toBase64());
	const auto auth = QStringLiteral("type=master&ver=1.0&sig=%1").arg(signature);
	return QString::fromUtf8(QUrl::toPercentEncoding(auth));
}
