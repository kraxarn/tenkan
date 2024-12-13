#pragma once

#include "api/api.hpp"
#include "config.hpp"

class CosmosDbApi final : public Api
{
	Q_OBJECT

public:
	CosmosDbApi(const Config &config, QObject *parent);

	void databases();

	void createDocument(const QString &id, const QString &value, const QDateTime &timestamp);

	void getDocument(const QString &id, std::function<void(QString)> &callback);

protected:
	[[nodiscard]]
	auto baseUrl() const -> QString override;

	[[nodiscard]]
	auto headers() const -> QHttpHeaders override;

private:
	const CosmosDbConfig config;
	QHttpHeaders requestHeaders;

	[[nodiscard]]
	auto createAuthSignature(const QString &method, const QString &resourceType,
		const QString &resourceLink, const QString &date) const -> QString;

	[[nodiscard]]
	static auto dateStr() -> QString;
};