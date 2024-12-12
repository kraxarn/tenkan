#pragma once

#include "api/api.hpp"
#include "data/nugetpackageinfo.hpp"

class NuGetApi final : Api
{
	Q_OBJECT

public:
	explicit NuGetApi(QObject *parent);

	void info(const QString &packageName, const std::function<void(NuGetPackageInfo)> &callback) const;

protected:
	[[nodiscard]]
	auto baseUrl() const -> QString override;

	[[nodiscard]]
	auto headers() const -> QHttpHeaders override;
};
