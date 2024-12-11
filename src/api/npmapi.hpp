#pragma once

#include "api/api.hpp"
#include "data/npmpackageinfo.hpp"

class NpmApi final : Api
{
	Q_OBJECT

public:
	explicit NpmApi(QObject *parent);

	void info(const QString &packageName, const std::function<void(NpmPackageInfo)> &callback) const;

protected:
	[[nodiscard]]
	auto baseUrl() const -> QString override;

	[[nodiscard]]
	auto headers() const -> QHttpHeaders override;
};
