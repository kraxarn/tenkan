#include "config.hpp"

#include <QJsonDocument>
#include <QFile>
#include <QCborMap>
#include <QJsonObject>
#include <QJsonArray>

#include "simplecrypt/simplecrypt.h"

#define SUPER_SECRET_KEY Q_UINT64_C(6168424485035677813)

Config::Config()
	: json(parse())
{
	{
		QFile file("config.cbor");
		file.open(QIODevice::WriteOnly);

		const auto value = QCborValue::fromJsonValue(json.object());
		file.write(value.toCbor());
	}
	{
		SimpleCrypt crypt(SUPER_SECRET_KEY);

		QFile file("config.bin");
		file.open(QIODevice::WriteOnly);

		const auto value = QCborValue::fromJsonValue(json.object());
		file.write(crypt.encryptToByteArray(value.toCbor()));
	}
}

auto Config::aikido() const -> AikidoConfig
{
	const auto object = json[QStringLiteral("aikido")].toObject();

	return {
		.clientId = object[QStringLiteral("client_id")].toString(),
		.clientSecret = object[QStringLiteral("client_secret")].toString(),
	};
}

auto Config::devops() const -> DevOpsConfig
{
	const auto object = json[QStringLiteral("devops")].toObject();

	return {
		.pat = object[QStringLiteral("pat")].toString(),
		.organization = object[QStringLiteral("organization")].toString(),
		.project = object[QStringLiteral("project")].toString(),
		.repositories = repositories(),
	};
}

auto Config::repositories() const -> QList<RepositoryConfig>
{
	const auto object = json[QStringLiteral("devops")].toObject();
	const auto array = object[QStringLiteral("repositories")].toArray();

	QList<RepositoryConfig> repositories;
	repositories.reserve(array.size());

	for (const auto &value: array)
	{
		const auto fileArray = value[QStringLiteral("files")].toArray();

		QList<QString> files;
		files.reserve(fileArray.size());

		for (const auto &fileValue: fileArray)
		{
			files.append(fileValue.toString());
		}

		repositories.append({
			.id = value[QStringLiteral("id")].toString(),
			.files = std::move(files),
		});
	}

	return repositories;
}

auto Config::parse() -> QJsonDocument
{
	if (QFile file(QStringLiteral("config.json"));
		file.open(QIODevice::ReadOnly))
	{
		qInfo() << "Reading config from json";

		return QJsonDocument::fromJson(file.readAll());
	}

	if (QFile file(QStringLiteral("config.cbor"));
		file.open(QIODevice::ReadOnly))
	{
		qInfo() << "Reading config from cbor";

		const auto value = QCborValue::fromCbor(file.readAll());
		const auto json = value.toMap().toJsonObject();
		return QJsonDocument(json);
	}

	if (QFile file(QStringLiteral("config.bin"));
		file.open(QIODevice::ReadOnly))
	{
		qInfo() << "Reading config from encrypted binary";

		SimpleCrypt crypt(SUPER_SECRET_KEY);
		const auto cbor = crypt.decryptToByteArray(file.readAll());
		const auto value = QCborValue::fromCbor(cbor);
		const auto json = value.toMap().toJsonObject();
		return QJsonDocument(json);
	}

	qFatal() << "No config file found";
	return {};
}
