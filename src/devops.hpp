#pragma once

#include <QNetworkAccessManager>

#include "config.hpp"
#include "data/dotnet/packagereference.hpp"
#include "data/nodejs/package.hpp"

class DevOps : public QObject
{
	Q_OBJECT

public:
	DevOps(const Config &config, QObject *parent);

	void getPackageReferences(const std::function<void(QList<DotNet::PackageReference>)> &callback) const;

	[[nodiscard]]
	auto getPackages() const -> QList<NodeJs::Package>;

private:
	const DevOpsConfig config;
	QNetworkAccessManager *manager;

	auto prepareRequest(const QString &url) const -> QNetworkRequest;

	void getFileContent(const QString &path, const std::function<void(QByteArray)> &callback) const;

	void await(QNetworkReply *reply, const std::function<void(QByteArray)> &callback) const;

	auto baseUrl() const -> QString;

	auto accessToken() const -> QString;

	auto repositoryId() const -> QString;

	auto projects() const -> const QStringList &;
};
