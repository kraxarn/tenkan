#pragma once

#include "api/api.hpp"
#include "config.hpp"
#include "data/aikidopackage.hpp"
#include "data/aikidocoderepository.hpp"
#include "config/aikido.hpp"

class AikidoApi : public Api
{
	Q_OBJECT

public:
	AikidoApi(const Config &config, QObject *parent);

	void packages(qint32 codeRepoId, const std::function<void(QList<AikidoPackage>)> &callback);

	void repositories(const std::function<void(QList<AikidoCodeRepository>)> &callback);

protected:
	[[nodiscard]]
	auto baseUrl() const -> QString override;

	[[nodiscard]]
	auto headers() const -> QHttpHeaders override;

private:
	const AikidoConfig config;

	QString token;

	void auth(const std::function<void(QString)> &callback);
};
