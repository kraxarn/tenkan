#pragma once

#include <QJsonDocument>

#include "config/repository.hpp"
#include "config/aikido.hpp"
#include "config/devops.hpp"
#include "config/cosmosdb.hpp"

class Config final
{
public:
	Config();

	[[nodiscard]]
	auto aikido() const -> AikidoConfig;

	[[nodiscard]]
	auto devops() const -> DevOpsConfig;

	[[nodiscard]]
	auto cosmosDb() const -> CosmosDbConfig;

private:
	const QJsonDocument json;

	[[nodiscard]]
	auto repositories() const -> QList<RepositoryConfig>;

	[[nodiscard]]
	static auto parse() -> QJsonDocument;

	void save() const;
};
