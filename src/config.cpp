#include "config.hpp"

#include <QJsonDocument>
#include <QFile>
#include <QCborMap>
#include <QJsonObject>
#include <QJsonArray>

Config::Config()
	: json(parse())
{
#ifndef NDEBUG
	save();
#endif
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
		.projects = projects(),
	};
}

auto Config::cosmosDb() const -> CosmosDbConfig
{
	const auto object = json[QStringLiteral("cosmos_db")].toObject();

	return {
		.key = object[QStringLiteral("key")].toString(),
		.name = object[QStringLiteral("name")].toString(),
		.database = object[QStringLiteral("database")].toString(),
		.container = object[QStringLiteral("container")].toString(),
	};
}

auto Config::projects() const -> QList<ProjectConfig>
{
	const auto object = json[QStringLiteral("devops")].toObject();
	const auto array = object[QStringLiteral("projects")].toArray();

	QList<ProjectConfig> projects;
	projects.reserve(array.size());

	for (const auto value : array)
	{
		projects.append({
			.id = value[QStringLiteral("id")].toString(),
			.repositories = repositories(value[QStringLiteral("repositories")].toArray()),
		});
	}

	return projects;
}

auto Config::repositories(const QJsonArray &array) -> QList<RepositoryConfig>
{
	QList<RepositoryConfig> repositories;
	repositories.reserve(array.size());

	for (const auto &value : array)
	{
		const auto fileArray = value[QStringLiteral("files")].toArray();

		QList<QString> files;
		files.reserve(fileArray.size());

		for (const auto &fileValue : fileArray)
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
		qInfo() << "Reading config from compressed binary";

		const auto cbor = qUncompress(file.readAll());
		const auto value = QCborValue::fromCbor(cbor);
		const auto json = value.toMap().toJsonObject();
		return QJsonDocument(json);
	}

	qFatal() << "No config file found";
	return {};
}

void Config::save() const
{
	QFile file("config.bin");
	file.open(QIODevice::WriteOnly);

	const auto value = QCborValue::fromJsonValue(json.object());
	file.write(qCompress(value.toCbor()));
}
