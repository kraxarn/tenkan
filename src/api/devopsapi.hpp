#pragma once

#include "config.hpp"
#include "data/dotnet/packagereference.hpp"
#include "data/nodejs/package.hpp"
#include "api/api.hpp"

class DevOpsApi : public Api
{
	Q_OBJECT

public:
	DevOpsApi(const Config &config, QObject *parent);

	void getPackageReferences(const std::function<void(QList<DotNet::PackageReference>)> &callback) const;

	[[nodiscard]]
	auto getPackages() const -> QList<NodeJs::Package>;

protected:
	[[nodiscard]]
	auto baseUrl() const -> QString override;

	[[nodiscard]]
	auto headers() const -> QHttpHeaders override;

private:
	const DevOpsConfig config;

	void getFileContent(const QString &path, const std::function<void(QByteArray)> &callback) const;

	auto accessToken() const -> QString;

	auto repositoryId() const -> QString;

	auto projects() const -> const QStringList &;
};
